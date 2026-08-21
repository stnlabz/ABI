#ifndef STNLABZ_MODULE_H
#define STNLABZ_MODULE_H

/*
 * STN-LABZ
 * Module ABI 1.4
 *
 * module.h
 *
 * Cross-application module ABI definitions.
 */

#define STNLABZ_MODULE_ID_MAX 64
#define STNLABZ_MODULE_NAME_MAX 64
#define STNLABZ_MODULE_COMMAND_NAME_MAX 64
#define STNLABZ_MODULE_COMMAND_ARGUMENTS_MAX 512
#define STNLABZ_MODULE_COMMAND_SENDER_MAX 128
#define STNLABZ_MODULE_COMMAND_ACCOUNT_MAX 128
#define STNLABZ_MODULE_MIN_TESTS 10


/*
 * ------------------------------------------------
 * CORE MODULE API
 * ------------------------------------------------
 */

#define STNLABZ_MODULE_API_MAJOR 1
#define STNLABZ_MODULE_API_MINOR 4


/*
 * ------------------------------------------------
 * MODULE LIFECYCLE
 * ------------------------------------------------
 *
 * ABI 1.4 adds:
 *
 *     STOPPED
 *     UNREGISTERED
 *
 * STOPPED is a persistent registry state.
 *
 * UNREGISTERED is the terminal Core-owned removal
 * transition. Core audits the transition before
 * removing the module record from the registry.
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
 * MODULE RESULTS
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
 * QUALIFICATION RESULT
 * ------------------------------------------------
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
 * GENERIC COMMAND ABI
 * ------------------------------------------------
 *
 * Retained for compatibility with hosts that use
 * command-oriented modules.
 *
 * Hosts that do not expose command services may
 * leave the corresponding host callbacks NULL.
 */

typedef struct
{
    char sender[
        STNLABZ_MODULE_COMMAND_SENDER_MAX
    ];

    char account[
        STNLABZ_MODULE_COMMAND_ACCOUNT_MAX
    ];

    char name[
        STNLABZ_MODULE_COMMAND_NAME_MAX
    ];

    char arguments[
        STNLABZ_MODULE_COMMAND_ARGUMENTS_MAX
    ];

} stnlabz_module_command_t;


typedef int
(*stnlabz_module_command_reply_fn)(
    void *reply_context,
    const char *message
);


typedef stnlabz_module_result_t
(*stnlabz_module_command_handler_fn)(
    const stnlabz_module_command_t *command,
    stnlabz_module_command_reply_fn reply,
    void *reply_context,
    void *handler_context
);


/*
 * ------------------------------------------------
 * CORE HOST SERVICES
 * ------------------------------------------------
 */

typedef int
(*stnlabz_module_send_message_fn)(
    const char *message
);


typedef int
(*stnlabz_module_register_command_fn)(
    const char *name,
    stnlabz_module_command_handler_fn handler,
    void *handler_context
);


typedef int
(*stnlabz_module_unregister_command_fn)(
    const char *name,
    void *handler_context
);


typedef struct
{
    stnlabz_module_send_message_fn
        send_message;

    stnlabz_module_register_command_fn
        register_command;

    stnlabz_module_unregister_command_fn
        unregister_command;

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
(*stnlabz_module_stop_fn)(void);


/*
 * ------------------------------------------------
 * MODULE DESCRIPTOR
 * ------------------------------------------------
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


const char *
stnlabz_module_state_string(
    stnlabz_module_state_t state
);


const char *
stnlabz_module_result_string(
    stnlabz_module_result_t result
);


#endif
