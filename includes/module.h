#ifndef STNLABZ_MODULE_H
#define STNLABZ_MODULE_H

/*
 * STN-LABZ
 * Module ABI 1.4
 *
 * module.h
 *
 * Cross-application module contract.
 */

#include <stddef.h>


#define STNLABZ_MODULE_ID_MAX       64
#define STNLABZ_MODULE_NAME_MAX     64
#define STNLABZ_MODULE_PATH_MAX     1024
#define STNLABZ_MODULE_SHA256_HEX   65
#define STNLABZ_MODULE_MIN_TESTS    10


/*
 * ------------------------------------------------
 * MODULE API VERSION
 * ------------------------------------------------
 *
 * ABI package revision 1.4 retains descriptor
 * compatibility with Module API 1.2.
 */

#define STNLABZ_MODULE_API_MAJOR    1
#define STNLABZ_MODULE_API_MINOR    2


/*
 * ------------------------------------------------
 * MODULE RESULT
 * ------------------------------------------------
 */

typedef enum
{
    STNLABZ_MODULE_OK = 0,

    STNLABZ_MODULE_ERR_INVALID_ARGUMENT,

    STNLABZ_MODULE_ERR_INVALID_IDENTITY,

    STNLABZ_MODULE_ERR_DUPLICATE,

    STNLABZ_MODULE_ERR_REGISTRY_FULL,

    STNLABZ_MODULE_ERR_NOT_FOUND,

    STNLABZ_MODULE_ERR_INCOMPATIBLE,

    STNLABZ_MODULE_ERR_INVALID_STATE,

    STNLABZ_MODULE_ERR_QUALIFICATION,

    STNLABZ_MODULE_ERR_NOT_QUALIFIED,

    STNLABZ_MODULE_ERR_NOT_AUTHORIZED,

    STNLABZ_MODULE_ERR_QUARANTINED,

    STNLABZ_MODULE_ERR_AUDIT_FULL,

    STNLABZ_MODULE_ERR_START_FAILED,

    STNLABZ_MODULE_ERR_STOP_FAILED

} stnlabz_module_result_t;


/*
 * ------------------------------------------------
 * MODULE STATE
 * ------------------------------------------------
 */

typedef enum
{
    STNLABZ_MODULE_STATE_DISCOVERED = 0,

    STNLABZ_MODULE_STATE_UNVERIFIED,

    STNLABZ_MODULE_STATE_TESTING,

    STNLABZ_MODULE_STATE_QUALIFIED,

    STNLABZ_MODULE_STATE_ACTIVE,

    STNLABZ_MODULE_STATE_STOPPED,

    STNLABZ_MODULE_STATE_FAILED,

    STNLABZ_MODULE_STATE_QUARANTINED,

    STNLABZ_MODULE_STATE_UNREGISTERED

} stnlabz_module_state_t;


/*
 * ------------------------------------------------
 * QUALIFICATION RESULT
 * ------------------------------------------------
 *
 * Qualification is evidence produced by the module
 * and evaluated by Core.
 *
 * A module does not determine its own qualification
 * state or activation authority.
 */

typedef struct
{
    unsigned int tests_executed;

    unsigned int tests_passed;

    unsigned int tests_failed;

    int negative_test_executed;

    int negative_test_passed;

} stnlabz_module_qualification_result_t;


/*
 * ------------------------------------------------
 * HOST INTERFACE
 * ------------------------------------------------
 *
 * The shared ABI does not define application-specific
 * host behavior.
 *
 * Applications provide their own context through this
 * boundary without changing the module descriptor.
 */

typedef struct stnlabz_module_host
{
    void *context;

} stnlabz_module_host_t;


/*
 * ------------------------------------------------
 * MODULE CALLBACKS
 * ------------------------------------------------
 */

typedef stnlabz_module_result_t
(*stnlabz_module_qualify_fn)(
    stnlabz_module_qualification_result_t *result
);


typedef stnlabz_module_result_t
(*stnlabz_module_start_fn)(
    const stnlabz_module_host_t *host
);


typedef stnlabz_module_result_t
(*stnlabz_module_stop_fn)(
    void
);


/*
 * ------------------------------------------------
 * MODULE DESCRIPTOR
 * ------------------------------------------------
 *
 * The descriptor is the stable identity and lifecycle
 * contract exported by a module.
 */

typedef struct
{
    char id[
        STNLABZ_MODULE_ID_MAX
    ];

    char name[
        STNLABZ_MODULE_NAME_MAX
    ];


    unsigned int version_major;

    unsigned int version_minor;

    unsigned int version_patch;


    unsigned int required_core_api_major;

    unsigned int required_core_api_minor;


    stnlabz_module_qualify_fn qualify;

    stnlabz_module_start_fn start;

    stnlabz_module_stop_fn stop;

} stnlabz_module_descriptor_t;


/*
 * ------------------------------------------------
 * MODULE UTILITIES
 * ------------------------------------------------
 */

const char *
stnlabz_module_state_string(
    stnlabz_module_state_t state
);


const char *
stnlabz_module_result_string(
    stnlabz_module_result_t result
);


#endif