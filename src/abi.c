/*
 * STN-LABZ
 * STN-LABZ Module ABI 1.3
 *
 * abi.c
 *
 * Reusable module lifecycle orchestration.
 *
 * Core owns lifecycle state, qualification decisions,
 * activation authority, and qualification evidence.
 *
 * Modules provide descriptors and callbacks.
 */

#include <stdio.h>
#include <string.h>

#include "abi.h"


/*
 * ------------------------------------------------
 * PREPARE MODULE
 * ------------------------------------------------
 */

stnlabz_module_result_t
stnlabz_module_abi_prepare(
    stnlabz_module_registry_t *registry,
    stnlabz_module_inventory_t *inventory,
    const stnlabz_module_descriptor_t *descriptor
)
{
    stnlabz_module_result_t result;

    const stnlabz_module_inventory_record_t
        *qualification_record;


    if (
        registry == NULL ||
        descriptor == NULL
    )
    {
        return
            STNLABZ_MODULE_ERR_INVALID_ARGUMENT;
    }


    /*
     * Discovery establishes presence only.
     */

    result =
        stnlabz_module_registry_discover(
            registry,
            descriptor
        );


    if (
        result !=
        STNLABZ_MODULE_OK
    )
    {
        return result;
    }


    /*
     * Verify identity and Core API compatibility.
     */

    result =
        stnlabz_module_registry_verify(
            registry,
            descriptor->id
        );


    if (
        result !=
        STNLABZ_MODULE_OK
    )
    {
        return result;
    }


    /*
     * If Core has persistent qualification evidence,
     * it may be restored only when inventory_find()
     * confirms an exact module revision and exact
     * required Core API match.
     */

    qualification_record =
        NULL;


    if (
        inventory != NULL
    )
    {
        qualification_record =
            stnlabz_module_inventory_find(
                inventory,
                descriptor
            );
    }


    if (
        qualification_record != NULL
    )
    {
        return
            stnlabz_module_registry_restore_qualification(
                registry,
                descriptor->id,
                &qualification_record->qualification
            );
    }


    /*
     * No exact persisted qualification exists.
     *
     * Execute the module qualification suite under
     * Core control.
     */

    result =
        stnlabz_module_registry_qualify(
            registry,
            descriptor->id
        );


    if (
        result !=
        STNLABZ_MODULE_OK
    )
    {
        return result;
    }


    /*
     * Persist live qualification evidence when an
     * inventory has been supplied.
     *
     * Inventory persistence failure does not erase the
     * live qualification result already established by
     * Core. The caller can decide whether persistence
     * failure is operationally fatal for the host.
     *
     * This preserves the existing separation between
     * module lifecycle results and inventory-specific
     * I/O results.
     */

    if (
        inventory != NULL
    )
    {
        const stnlabz_module_record_t *record;

        stnlabz_module_inventory_result_t
            inventory_result;


        record =
            stnlabz_module_registry_find(
                registry,
                descriptor->id
            );


        if (
            record == NULL
        )
        {
            (void)
            stnlabz_module_registry_fail(
                registry,
                descriptor->id
            );


            return
                STNLABZ_MODULE_ERR_NOT_FOUND;
        }


        inventory_result =
            stnlabz_module_inventory_store(
                inventory,
                descriptor,
                &record->qualification
            );


        if (
            inventory_result !=
            STNLABZ_MODULE_INVENTORY_OK
        )
        {
            /*
             * The current module result enum has no
             * inventory-I/O-specific result.
             *
             * Treat inability to preserve Core-owned
             * qualification evidence as a failed
             * preparation boundary.
             */

            (void)
            stnlabz_module_registry_fail(
                registry,
                descriptor->id
            );


            return
                STNLABZ_MODULE_ERR_QUALIFICATION;
        }
    }


    return
        STNLABZ_MODULE_OK;
}


/*
 * ------------------------------------------------
 * AUTHORIZE AND ACTIVATE
 * ------------------------------------------------
 */

stnlabz_module_result_t
stnlabz_module_abi_authorize_and_activate(
    stnlabz_module_registry_t *registry,
    const char *module_id,
    const stnlabz_module_host_t *host
)
{
    const stnlabz_module_record_t *record;

    stnlabz_module_result_t result;


    if (
        registry == NULL ||
        module_id == NULL ||
        module_id[0] == '\0'
    )
    {
        return
            STNLABZ_MODULE_ERR_INVALID_ARGUMENT;
    }


    record =
        stnlabz_module_registry_find(
            registry,
            module_id
        );


    if (
        record == NULL
    )
    {
        return
            STNLABZ_MODULE_ERR_NOT_FOUND;
    }


    /*
     * Authorization is separate from qualification.
     */

    result =
        stnlabz_module_registry_authorize_activation(
            registry,
            module_id
        );


    if (
        result !=
        STNLABZ_MODULE_OK
    )
    {
        return result;
    }


    /*
     * Start callback is optional in the current
     * descriptor structure.
     *
     * A module with no start callback may still be
     * activated if Core has qualified and authorized
     * it.
     */

    record =
        stnlabz_module_registry_find(
            registry,
            module_id
        );


    if (
        record == NULL
    )
    {
        return
            STNLABZ_MODULE_ERR_NOT_FOUND;
    }


    if (
        record->descriptor.start != NULL
    )
    {
        result =
            record
                ->descriptor
                .start(
                    host
                );


        if (
            result !=
            STNLABZ_MODULE_OK
        )
        {
            (void)
            stnlabz_module_registry_fail(
                registry,
                module_id
            );


            return
                STNLABZ_MODULE_ERR_START_FAILED;
        }
    }


    /*
     * Only after successful start does Core record the
     * module as ACTIVE.
     */

    result =
        stnlabz_module_registry_activate(
            registry,
            module_id
        );


    if (
        result !=
        STNLABZ_MODULE_OK
    )
    {
        /*
         * Start succeeded but ACTIVE state could not
         * be committed.
         *
         * Roll the module callback back before marking
         * the registry record FAILED.
         */

        record =
            stnlabz_module_registry_find(
                registry,
                module_id
            );


        if (
            record != NULL &&
            record->descriptor.stop != NULL
        )
        {
            (void)
            record
                ->descriptor
                .stop();
        }


        (void)
        stnlabz_module_registry_fail(
            registry,
            module_id
        );


        return result;
    }


    return
        STNLABZ_MODULE_OK;
}


/*
 * ------------------------------------------------
 * STOP CALLBACK
 * ------------------------------------------------
 */

stnlabz_module_result_t
stnlabz_module_abi_stop(
    stnlabz_module_registry_t *registry,
    const char *module_id
)
{
    const stnlabz_module_record_t *record;

    stnlabz_module_result_t result;


    if (
        registry == NULL ||
        module_id == NULL ||
        module_id[0] == '\0'
    )
    {
        return
            STNLABZ_MODULE_ERR_INVALID_ARGUMENT;
    }


    record =
        stnlabz_module_registry_find(
            registry,
            module_id
        );


    if (
        record == NULL
    )
    {
        return
            STNLABZ_MODULE_ERR_NOT_FOUND;
    }


    if (
        record->state ==
        STNLABZ_MODULE_STATE_QUARANTINED
    )
    {
        return
            STNLABZ_MODULE_ERR_QUARANTINED;
    }


    if (
        record->state !=
        STNLABZ_MODULE_STATE_ACTIVE
    )
    {
        return
            STNLABZ_MODULE_ERR_INVALID_STATE;
    }


    if (
        record->descriptor.stop ==
        NULL
    )
    {
        return
            STNLABZ_MODULE_OK;
    }


    result =
        record
            ->descriptor
            .stop();


    if (
        result !=
        STNLABZ_MODULE_OK
    )
    {
        (void)
        stnlabz_module_registry_fail(
            registry,
            module_id
        );


        return
            STNLABZ_MODULE_ERR_STOP_FAILED;
    }


    /*
     * ABI 1.3 contains no inactive/deactivated state.
     *
     * Do not fabricate a lifecycle transition here.
     */

    return
        STNLABZ_MODULE_OK;
}


/*
 * ================================================================
 * ABI SELF TEST
 * ================================================================
 */


/*
 * ------------------------------------------------
 * TEST MODULE QUALIFICATION
 * ------------------------------------------------
 */

static stnlabz_module_result_t
abi_test_qualify(
    stnlabz_module_qualification_result_t *result
)
{
    if (
        result == NULL
    )
    {
        return
            STNLABZ_MODULE_ERR_INVALID_ARGUMENT;
    }


    memset(
        result,
        0,
        sizeof(*result)
    );


    result->tests_executed =
        STNLABZ_MODULE_MIN_TESTS;

    result->tests_passed =
        STNLABZ_MODULE_MIN_TESTS;

    result->tests_failed =
        0;

    result->negative_test_executed =
        1;

    result->negative_test_passed =
        1;


    return
        STNLABZ_MODULE_OK;
}


/*
 * ------------------------------------------------
 * TEST MODULE START
 * ------------------------------------------------
 */

static stnlabz_module_result_t
abi_test_start(
    const stnlabz_module_host_t *host
)
{
    (void)host;


    return
        STNLABZ_MODULE_OK;
}


/*
 * ------------------------------------------------
 * TEST MODULE STOP
 * ------------------------------------------------
 */

static stnlabz_module_result_t
abi_test_stop(void)
{
    return
        STNLABZ_MODULE_OK;
}


/*
 * ------------------------------------------------
 * TEST DESCRIPTOR
 * ------------------------------------------------
 */

static const stnlabz_module_descriptor_t
ABI_TEST_DESCRIPTOR =
{
    "abi-test",

    "ABI Test Module",

    1,
    0,
    0,

    STNLABZ_MODULE_API_MAJOR,
    STNLABZ_MODULE_API_MINOR,

    abi_test_qualify,
    abi_test_start,
    abi_test_stop
};


/*
 * ------------------------------------------------
 * EXPECT RESULT
 * ------------------------------------------------
 */

static int
abi_test_expect(
    const char *name,
    stnlabz_module_result_t actual,
    stnlabz_module_result_t expected
)
{
    if (
        name == NULL
    )
    {
        return 0;
    }


    printf(
        "%-30s : %-20s",
        name,
        stnlabz_module_result_string(
            actual
        )
    );


    if (
        actual != expected
    )
    {
        printf(
            " [EXPECTED %s]\n",
            stnlabz_module_result_string(
                expected
            )
        );


        return 0;
    }


    printf(
        " PASS\n"
    );


    return 1;
}


/*
 * ------------------------------------------------
 * EXPECT STATE
 * ------------------------------------------------
 */

static int
abi_test_expect_state(
    const stnlabz_module_registry_t *registry,
    const char *module_id,
    stnlabz_module_state_t expected
)
{
    const stnlabz_module_record_t *record;


    record =
        stnlabz_module_registry_find(
            registry,
            module_id
        );


    if (
        record == NULL
    )
    {
        printf(
            "%-30s : NOT_FOUND FAIL\n",
            "STATE"
        );


        return 0;
    }


    printf(
        "%-30s : %-20s",
        "STATE",
        stnlabz_module_state_string(
            record->state
        )
    );


    if (
        record->state != expected
    )
    {
        printf(
            " [EXPECTED %s]\n",
            stnlabz_module_state_string(
                expected
            )
        );


        return 0;
    }


    printf(
        " PASS\n"
    );


    return 1;
}


/*
 * ------------------------------------------------
 * SELF TEST
 * ------------------------------------------------
 */

int
stnlabz_module_abi_self_test(void)
{
    stnlabz_module_registry_t registry;

    stnlabz_module_result_t result;


    printf(
        "STN-LABZ Module ABI %u.%u Self Test\n",
        STNLABZ_MODULE_API_MAJOR,
        STNLABZ_MODULE_API_MINOR
    );


    printf(
        "============================================================\n"
    );


    stnlabz_module_registry_init(
        &registry
    );


    /*
     * Prepare with no persistent inventory.
     *
     * This executes:
     *
     *     discover
     *     verify
     *     qualify
     */

    result =
        stnlabz_module_abi_prepare(
            &registry,
            NULL,
            &ABI_TEST_DESCRIPTOR
        );


    if (
        !abi_test_expect(
            "PREPARE",
            result,
            STNLABZ_MODULE_OK
        )
    )
    {
        return 1;
    }


    if (
        !abi_test_expect_state(
            &registry,
            ABI_TEST_DESCRIPTOR.id,
            STNLABZ_MODULE_STATE_QUALIFIED
        )
    )
    {
        return 1;
    }


    /*
     * Qualification alone must not activate.
     */

    result =
        stnlabz_module_registry_activate(
            &registry,
            ABI_TEST_DESCRIPTOR.id
        );


    if (
        !abi_test_expect(
            "ACTIVATE WITHOUT AUTHORITY",
            result,
            STNLABZ_MODULE_ERR_NOT_AUTHORIZED
        )
    )
    {
        return 1;
    }


    /*
     * Explicit authorization + callback start +
     * ACTIVE registry transition.
     */

    result =
        stnlabz_module_abi_authorize_and_activate(
            &registry,
            ABI_TEST_DESCRIPTOR.id,
            NULL
        );


    if (
        !abi_test_expect(
            "AUTHORIZE + ACTIVATE",
            result,
            STNLABZ_MODULE_OK
        )
    )
    {
        return 1;
    }


    if (
        !abi_test_expect_state(
            &registry,
            ABI_TEST_DESCRIPTOR.id,
            STNLABZ_MODULE_STATE_ACTIVE
        )
    )
    {
        return 1;
    }


    /*
     * Orderly module callback shutdown.
     *
     * ABI 1.3 leaves registry state ACTIVE because no
     * DEACTIVATED/STOPPED transition exists yet.
     */

    result =
        stnlabz_module_abi_stop(
            &registry,
            ABI_TEST_DESCRIPTOR.id
        );


    if (
        !abi_test_expect(
            "STOP CALLBACK",
            result,
            STNLABZ_MODULE_OK
        )
    )
    {
        return 1;
    }


    if (
        !abi_test_expect_state(
            &registry,
            ABI_TEST_DESCRIPTOR.id,
            STNLABZ_MODULE_STATE_ACTIVE
        )
    )
    {
        return 1;
    }


    /*
     * Exercise containment.
     */

    result =
        stnlabz_module_registry_quarantine(
            &registry,
            ABI_TEST_DESCRIPTOR.id
        );


    if (
        !abi_test_expect(
            "QUARANTINE",
            result,
            STNLABZ_MODULE_OK
        )
    )
    {
        return 1;
    }


    if (
        !abi_test_expect_state(
            &registry,
            ABI_TEST_DESCRIPTOR.id,
            STNLABZ_MODULE_STATE_QUARANTINED
        )
    )
    {
        return 1;
    }


    /*
     * Quarantined modules remain blocked.
     */

    result =
        stnlabz_module_registry_authorize_activation(
            &registry,
            ABI_TEST_DESCRIPTOR.id
        );


    if (
        !abi_test_expect(
            "AUTHORIZE QUARANTINED",
            result,
            STNLABZ_MODULE_ERR_QUARANTINED
        )
    )
    {
        return 1;
    }


    printf(
        "============================================================\n"
    );


    printf(
        "MODULE ABI SELF TEST PASS\n"
    );


    return 0;
}
