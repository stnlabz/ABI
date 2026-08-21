#ifndef STNLABZ_MODULE_REGISTRY_H
#define STNLABZ_MODULE_REGISTRY_H

/*
 * STN-LABZ
 * Module ABI 1.4
 *
 * module_registry.h
 *
 * Core-controlled module lifecycle registry.
 */

#include <stddef.h>

#include "module.h"


#define STNLABZ_MODULE_REGISTRY_MAX 32
#define STNLABZ_MODULE_AUDIT_MAX 128


/*
 * ------------------------------------------------
 * MODULE AUDIT EVENTS
 * ------------------------------------------------
 */

typedef enum
{
    STNLABZ_MODULE_AUDIT_DISCOVERED = 0,

    STNLABZ_MODULE_AUDIT_VERIFIED,

    STNLABZ_MODULE_AUDIT_TESTING,

    STNLABZ_MODULE_AUDIT_QUALIFIED,

    STNLABZ_MODULE_AUDIT_FAILED,

    STNLABZ_MODULE_AUDIT_AUTHORIZED,

    STNLABZ_MODULE_AUDIT_ACTIVE,

    STNLABZ_MODULE_AUDIT_STOPPED,

    STNLABZ_MODULE_AUDIT_QUARANTINED,

    STNLABZ_MODULE_AUDIT_UNREGISTERED

} stnlabz_module_audit_event_t;


/*
 * ------------------------------------------------
 * MODULE RECORD
 * ------------------------------------------------
 */

typedef struct
{
    stnlabz_module_descriptor_t descriptor;

    stnlabz_module_state_t state;

    stnlabz_module_qualification_result_t qualification;

    int activation_authorized;

} stnlabz_module_record_t;


/*
 * ------------------------------------------------
 * MODULE AUDIT ENTRY
 * ------------------------------------------------
 */

typedef struct
{
    unsigned long sequence;

    char module_id[
        STNLABZ_MODULE_ID_MAX
    ];

    stnlabz_module_audit_event_t event;

    stnlabz_module_state_t previous_state;

    stnlabz_module_state_t resulting_state;

    stnlabz_module_result_t result;

} stnlabz_module_audit_entry_t;


/*
 * ------------------------------------------------
 * MODULE REGISTRY
 * ------------------------------------------------
 */

typedef struct
{
    stnlabz_module_record_t modules[
        STNLABZ_MODULE_REGISTRY_MAX
    ];

    size_t count;

    stnlabz_module_audit_entry_t audit[
        STNLABZ_MODULE_AUDIT_MAX
    ];

    size_t audit_count;

    unsigned long next_sequence;

} stnlabz_module_registry_t;


/*
 * ------------------------------------------------
 * REGISTRY API
 * ------------------------------------------------
 */

void
stnlabz_module_registry_init(
    stnlabz_module_registry_t *registry
);


stnlabz_module_result_t
stnlabz_module_registry_discover(
    stnlabz_module_registry_t *registry,
    const stnlabz_module_descriptor_t *descriptor
);


stnlabz_module_result_t
stnlabz_module_registry_verify(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


stnlabz_module_result_t
stnlabz_module_registry_qualify(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


stnlabz_module_result_t
stnlabz_module_registry_restore_qualification(
    stnlabz_module_registry_t *registry,
    const char *module_id,
    const stnlabz_module_qualification_result_t *qualification
);


stnlabz_module_result_t
stnlabz_module_registry_authorize_activation(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


stnlabz_module_result_t
stnlabz_module_registry_activate(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


/*
 * ACTIVE -> STOPPED
 *
 * Qualification evidence is retained.
 * Activation authority is cleared.
 */
stnlabz_module_result_t
stnlabz_module_registry_stop(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


/*
 * Remove a non-running module from the registry.
 *
 * Core first records an UNREGISTERED audit event,
 * then removes the module record.
 *
 * ACTIVE modules cannot be unregistered.
 */
stnlabz_module_result_t
stnlabz_module_registry_unregister(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


stnlabz_module_result_t
stnlabz_module_registry_fail(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


stnlabz_module_result_t
stnlabz_module_registry_quarantine(
    stnlabz_module_registry_t *registry,
    const char *module_id
);


const stnlabz_module_record_t *
stnlabz_module_registry_find(
    const stnlabz_module_registry_t *registry,
    const char *module_id
);


#endif
