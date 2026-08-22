/*
 * STN-LABZ
 * Module ABI
 *
 * module_inventory.c
 *
 * Persistent Core-owned module qualification
 * evidence.
 *
 * The module declares what it is.
 * Core records what it has demonstrated.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "module_inventory.h"


#define STNLABZ_MODULE_INVENTORY_FILENAME \
    "module_inventory.conf"


/*
 * ------------------------------------------------
 * RECORD VALIDATION
 * ------------------------------------------------
 */

static int stnlabz_module_inventory_record_valid(
    const stnlabz_module_inventory_record_t *record
)
{
    if (
        record == NULL
    )
    {
        return 0;
    }


    if (
        record->module_id[0] ==
        '\0'
    )
    {
        return 0;
    }


    if (
        record
            ->qualification
            .tests_executed <
        STNLABZ_MODULE_MIN_TESTS
    )
    {
        return 0;
    }


    if (
        record
            ->qualification
            .tests_passed !=
        record
            ->qualification
            .tests_executed
    )
    {
        return 0;
    }


    if (
        record
            ->qualification
            .tests_failed != 0
    )
    {
        return 0;
    }


    if (
        record
            ->qualification
            .tests_passed +
        record
            ->qualification
            .tests_failed !=
        record
            ->qualification
            .tests_executed
    )
    {
        return 0;
    }


    if (
        !record
            ->qualification
            .negative_test_executed ||
        !record
            ->qualification
            .negative_test_passed
    )
    {
        return 0;
    }


    return 1;
}


/*
 * ------------------------------------------------
 * SAVE
 * ------------------------------------------------
 */

static stnlabz_module_inventory_result_t
stnlabz_module_inventory_save(
    const stnlabz_module_inventory_t *inventory
)
{
    FILE *file =
        NULL;

    size_t index;


    if (
        inventory == NULL ||
        inventory->path[0] ==
            '\0'
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT;
    }


    if (
        fopen_s(
            &file,
            inventory->path,
            "w"
        ) != 0 ||
        file == NULL
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_OPEN_FAILED;
    }


    for (
        index = 0;
        index < inventory->count;
        ++index
    )
    {
        const stnlabz_module_inventory_record_t *record;

        int written;


        record =
            &inventory->records[index];


        written =
            fprintf(
                file,
                "%s|%u|%u|%u|%u|%u|%u|%u|%u|%d|%d\n",

                record->module_id,

                record->version_major,
                record->version_minor,
                record->version_patch,

                record->core_api_major,
                record->core_api_minor,

                record
                    ->qualification
                    .tests_executed,

                record
                    ->qualification
                    .tests_passed,

                record
                    ->qualification
                    .tests_failed,

                record
                    ->qualification
                    .negative_test_executed,

                record
                    ->qualification
                    .negative_test_passed
            );


        if (
            written < 0
        )
        {
            fclose(
                file
            );

            return
                STNLABZ_MODULE_INVENTORY_ERR_WRITE_FAILED;
        }
    }


    if (
        fflush(
            file
        ) != 0
    )
    {
        fclose(
            file
        );

        return
            STNLABZ_MODULE_INVENTORY_ERR_WRITE_FAILED;
    }


    if (
        fclose(
            file
        ) != 0
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_WRITE_FAILED;
    }


    return
        STNLABZ_MODULE_INVENTORY_OK;
}


/*
 * ------------------------------------------------
 * INITIALIZE
 * ------------------------------------------------
 */

void stnlabz_module_inventory_init(
    stnlabz_module_inventory_t *inventory
)
{
    if (
        inventory == NULL
    )
    {
        return;
    }


    memset(
        inventory,
        0,
        sizeof(*inventory)
    );
}


/*
 * ------------------------------------------------
 * CONFIGURE
 * ------------------------------------------------
 *
 * state_path is Core-owned mutable state storage.
 *
 * The inventory is not stored inside an individual
 * module directory.
 */

stnlabz_module_inventory_result_t
stnlabz_module_inventory_configure(
    stnlabz_module_inventory_t *inventory,
    const char *state_path
)
{
    int written;


    if (
        inventory == NULL ||
        state_path == NULL ||
        state_path[0] == '\0'
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT;
    }


    written =
        snprintf(
            inventory->path,
            sizeof(inventory->path),
            "%s\\%s",
            state_path,
            STNLABZ_MODULE_INVENTORY_FILENAME
        );


    if (
        written < 0 ||
        (size_t)written >=
            sizeof(inventory->path)
    )
    {
        inventory->path[0] =
            '\0';

        return
            STNLABZ_MODULE_INVENTORY_ERR_PATH_TOO_LONG;
    }


    return
        STNLABZ_MODULE_INVENTORY_OK;
}


/*
 * ------------------------------------------------
 * LOAD
 * ------------------------------------------------
 *
 * Missing inventory is valid.
 *
 * It means Core has no persistent qualification
 * evidence and modules must qualify normally.
 *
 * Malformed inventory is rejected in its entirety.
 */

stnlabz_module_inventory_result_t
stnlabz_module_inventory_load(
    stnlabz_module_inventory_t *inventory
)
{
    FILE *file =
        NULL;

    char line[
        512
    ];


    if (
        inventory == NULL ||
        inventory->path[0] ==
            '\0'
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT;
    }


    inventory->count =
        0;


    if (
        fopen_s(
            &file,
            inventory->path,
            "r"
        ) != 0 ||
        file == NULL
    )
    {
        /*
         * First run:
         *
         * no qualification inventory yet.
         */

        return
            STNLABZ_MODULE_INVENTORY_OK;
    }


    while (
        fgets(
            line,
            sizeof(line),
            file
        ) != NULL
    )
    {
        stnlabz_module_inventory_record_t record;

        int fields;


        memset(
            &record,
            0,
            sizeof(record)
        );


        fields =
            sscanf_s(
                line,

                "%63[^|]|%u|%u|%u|%u|%u|%u|%u|%u|%d|%d",

                record.module_id,
                (unsigned int)
                    sizeof(record.module_id),

                &record.version_major,
                &record.version_minor,
                &record.version_patch,

                &record.core_api_major,
                &record.core_api_minor,

                &record
                    .qualification
                    .tests_executed,

                &record
                    .qualification
                    .tests_passed,

                &record
                    .qualification
                    .tests_failed,

                &record
                    .qualification
                    .negative_test_executed,

                &record
                    .qualification
                    .negative_test_passed
            );


        if (
            fields != 11
        )
        {
            fclose(
                file
            );

            inventory->count =
                0;

            return
                STNLABZ_MODULE_INVENTORY_ERR_INVALID_FORMAT;
        }


        if (
            !stnlabz_module_inventory_record_valid(
                &record
            )
        )
        {
            fclose(
                file
            );

            inventory->count =
                0;

            return
                STNLABZ_MODULE_INVENTORY_ERR_INVALID_FORMAT;
        }


        if (
            inventory->count >=
            STNLABZ_MODULE_INVENTORY_MAX
        )
        {
            fclose(
                file
            );

            inventory->count =
                0;

            return
                STNLABZ_MODULE_INVENTORY_ERR_FULL;
        }


        inventory->records[
            inventory->count
        ] =
            record;


        inventory->count++;
    }


    if (
        ferror(
            file
        )
    )
    {
        fclose(
            file
        );

        inventory->count =
            0;

        return
            STNLABZ_MODULE_INVENTORY_ERR_READ_FAILED;
    }


    fclose(
        file
    );


    return
        STNLABZ_MODULE_INVENTORY_OK;
}


/*
 * ------------------------------------------------
 * STORE QUALIFICATION
 * ------------------------------------------------
 */

stnlabz_module_inventory_result_t
stnlabz_module_inventory_store(
    stnlabz_module_inventory_t *inventory,
    const stnlabz_module_descriptor_t *descriptor,
    const stnlabz_module_qualification_result_t *qualification
)
{
    stnlabz_module_inventory_record_t *record =
        NULL;

    size_t index;

    size_t length;


    if (
        inventory == NULL ||
        descriptor == NULL ||
        qualification == NULL
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT;
    }


    /*
     * Existing record is replaced only when the
     * same module ID is being persisted again.
     */

    for (
        index = 0;
        index < inventory->count;
        ++index
    )
    {
        if (
            strcmp(
                inventory
                    ->records[index]
                    .module_id,

                descriptor->id
            ) == 0
        )
        {
            record =
                &inventory->records[index];

            break;
        }
    }


    if (
        record == NULL
    )
    {
        if (
            inventory->count >=
            STNLABZ_MODULE_INVENTORY_MAX
        )
        {
            return
                STNLABZ_MODULE_INVENTORY_ERR_FULL;
        }


        record =
            &inventory->records[
                inventory->count
            ];


        inventory->count++;
    }


    memset(
        record,
        0,
        sizeof(*record)
    );


    length =
        strlen(
            descriptor->id
        );


    if (
        length == 0 ||
        length >=
            sizeof(record->module_id)
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT;
    }


    memcpy(
        record->module_id,
        descriptor->id,
        length + 1
    );


    record->version_major =
        descriptor->version_major;

    record->version_minor =
        descriptor->version_minor;

    record->version_patch =
        descriptor->version_patch;


    record->core_api_major =
        descriptor->required_core_api_major;

    record->core_api_minor =
        descriptor->required_core_api_minor;


    record->qualification =
        *qualification;


    if (
        !stnlabz_module_inventory_record_valid(
            record
        )
    )
    {
        return
            STNLABZ_MODULE_INVENTORY_ERR_INVALID_FORMAT;
    }


    return
        stnlabz_module_inventory_save(
            inventory
        );
}


/*
 * ------------------------------------------------
 * FIND MATCHING QUALIFICATION
 * ------------------------------------------------
 *
 * Qualification is valid only for the exact module
 * revision and exact required Core API.
 *
 * A changed module revision does not inherit trust
 * from an older revision.
 */

const stnlabz_module_inventory_record_t *
stnlabz_module_inventory_find(
    const stnlabz_module_inventory_t *inventory,
    const stnlabz_module_descriptor_t *descriptor
)
{
    size_t index;


    if (
        inventory == NULL ||
        descriptor == NULL
    )
    {
        return NULL;
    }


    for (
        index = 0;
        index < inventory->count;
        ++index
    )
    {
        const stnlabz_module_inventory_record_t *record;


        record =
            &inventory->records[index];


        if (
            strcmp(
                record->module_id,
                descriptor->id
            ) != 0
        )
        {
            continue;
        }


        /*
         * Module revision must match exactly.
         */

        if (
            record->version_major !=
                descriptor->version_major ||
            record->version_minor !=
                descriptor->version_minor ||
            record->version_patch !=
                descriptor->version_patch
        )
        {
            return NULL;
        }


        /*
         * Required Core API must match the
         * qualification evidence.
         */

        if (
            record->core_api_major !=
                descriptor
                    ->required_core_api_major ||
            record->core_api_minor !=
                descriptor
                    ->required_core_api_minor
        )
        {
            return NULL;
        }


        if (
            !stnlabz_module_inventory_record_valid(
                record
            )
        )
        {
            return NULL;
        }


        return record;
    }


    return NULL;
}


/*
 * ------------------------------------------------
 * RESULT STRING
 * ------------------------------------------------
 */

const char *stnlabz_module_inventory_result_string(
    stnlabz_module_inventory_result_t result
)
{
    switch (
        result
    )
    {
        case STNLABZ_MODULE_INVENTORY_OK:

            return "OK";


        case STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT:

            return "INVALID_ARGUMENT";


        case STNLABZ_MODULE_INVENTORY_ERR_PATH_TOO_LONG:

            return "PATH_TOO_LONG";


        case STNLABZ_MODULE_INVENTORY_ERR_OPEN_FAILED:

            return "OPEN_FAILED";


        case STNLABZ_MODULE_INVENTORY_ERR_READ_FAILED:

            return "READ_FAILED";


        case STNLABZ_MODULE_INVENTORY_ERR_WRITE_FAILED:

            return "WRITE_FAILED";


        case STNLABZ_MODULE_INVENTORY_ERR_INVALID_FORMAT:

            return "INVALID_FORMAT";


        case STNLABZ_MODULE_INVENTORY_ERR_FULL:

            return "FULL";


        default:

            return "UNKNOWN";
    }
}