#ifndef STNLABZ_MODULE_ABI_H
#define STNLABZ_MODULE_ABI_H

/*
 * STN-LABZ
 * STN-LABZ Module ABI 1.3
 *
 * abi.h
 *
 * Reusable module lifecycle orchestration.
 *
 * This layer sits above the Core-owned registry and
 * qualification inventory.
 *
 * It does not perform filesystem discovery or dynamic
 * library loading. Those remain platform-specific.
 */

#include "module.h"
#include "module_inventory.h"
#include "module_registry.h"


/*
 * ------------------------------------------------
 * ABI RESULT
 * ------------------------------------------------
 *
 * The orchestration layer returns the established
 * module result type so callers do not need a second
 * result namespace for lifecycle operations.
 */


/*
 * ------------------------------------------------
 * PREPARE MODULE
 * ------------------------------------------------
 *
 * Process a descriptor through the normal Core-owned
 * lifecycle:
 *
 *     DISCOVER
 *       -> VERIFY
 *       -> restore exact qualification evidence
 *          OR execute live qualification
 *       -> persist live qualification evidence
 *
 * Qualification restoration is attempted only when an
 * inventory is supplied and an exact record exists for:
 *
 *     module ID
 *     module version
 *     required Core API version
 *
 * This function does not authorize activation and does
 * not start the module.
 */
stnlabz_module_result_t
stnlabz_module_abi_prepare(
    stnlabz_module_registry_t *registry,
    stnlabz_module_inventory_t *inventory,
    const stnlabz_module_descriptor_t *descriptor
);


/*
 * ------------------------------------------------
 * AUTHORIZE AND ACTIVATE
 * ------------------------------------------------
 *
 * Explicitly authorize activation, call the module
 * start callback, then record ACTIVE state in the
 * Core registry.
 *
 * If start fails, Core marks the module FAILED.
 *
 * If registry activation fails after start succeeds,
 * the stop callback is invoked as rollback when one is
 * available, then Core marks the module FAILED.
 */
stnlabz_module_result_t
stnlabz_module_abi_authorize_and_activate(
    stnlabz_module_registry_t *registry,
    const char *module_id,
    const stnlabz_module_host_t *host
);


/*
 * ------------------------------------------------
 * STOP CALLBACK
 * ------------------------------------------------
 *
 * Invoke the module stop callback for an ACTIVE module.
 *
 * IMPORTANT:
 *
 * ABI 1.3 does not currently define a DEACTIVATED,
 * STOPPED, or UNREGISTERED lifecycle state. Therefore
 * this function does not alter registry state after a
 * successful stop callback.
 *
 * It is suitable for orderly application shutdown.
 *
 * True hot replacement requires a future Core registry
 * transition that can safely remove an ACTIVE module
 * before its shared library is unloaded.
 */
stnlabz_module_result_t
stnlabz_module_abi_stop(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


/*
 * ------------------------------------------------
 * SELF TEST
 * ------------------------------------------------
 *
 * Deterministic standalone validation of the reusable
 * ABI orchestration layer.
 *
 * Returns 0 on PASS and non-zero on failure.
 */
int
stnlabz_module_abi_self_test(void);


#endif
