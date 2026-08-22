#ifndef STNLABZ_MODULE_INVENTORY_H
#define STNLABZ_MODULE_INVENTORY_H

#include <stddef.h>

#include "module.h"


#define STNLABZ_MODULE_INVENTORY_MAX \
    32

#define STNLABZ_MODULE_INVENTORY_PATH_MAX \
    1024


typedef enum
{
    STNLABZ_MODULE_INVENTORY_OK = 0,

    STNLABZ_MODULE_INVENTORY_ERR_INVALID_ARGUMENT,

    STNLABZ_MODULE_INVENTORY_ERR_PATH_TOO_LONG,

    STNLABZ_MODULE_INVENTORY_ERR_OPEN_FAILED,

    STNLABZ_MODULE_INVENTORY_ERR_READ_FAILED,

    STNLABZ_MODULE_INVENTORY_ERR_WRITE_FAILED,

    STNLABZ_MODULE_INVENTORY_ERR_INVALID_FORMAT,

    STNLABZ_MODULE_INVENTORY_ERR_FULL

} stnlabz_module_inventory_result_t;


/*
 * ------------------------------------------------
 * PERSISTED QUALIFICATION RECORD
 * ------------------------------------------------
 *
 * Core owns this evidence.
 *
 * The module does not write, modify, or determine
 * this record.
 */

typedef struct
{
    char module_id[
        STNLABZ_MODULE_ID_MAX
    ];

    unsigned int version_major;

    unsigned int version_minor;

    unsigned int version_patch;


    unsigned int core_api_major;

    unsigned int core_api_minor;


    stnlabz_module_qualification_result_t qualification;

} stnlabz_module_inventory_record_t;


/*
 * ------------------------------------------------
 * INVENTORY
 * ------------------------------------------------
 */

typedef struct
{
    stnlabz_module_inventory_record_t
        records[
            STNLABZ_MODULE_INVENTORY_MAX
        ];

    size_t count;


    char path[
        STNLABZ_MODULE_INVENTORY_PATH_MAX
    ];

} stnlabz_module_inventory_t;


/*
 * ------------------------------------------------
 * API
 * ------------------------------------------------
 */

void stnlabz_module_inventory_init(
    stnlabz_module_inventory_t *inventory
);


stnlabz_module_inventory_result_t
stnlabz_module_inventory_configure(
    stnlabz_module_inventory_t *inventory,
    const char *state_path
);


stnlabz_module_inventory_result_t
stnlabz_module_inventory_load(
    stnlabz_module_inventory_t *inventory
);


stnlabz_module_inventory_result_t
stnlabz_module_inventory_store(
    stnlabz_module_inventory_t *inventory,
    const stnlabz_module_descriptor_t *descriptor,
    const stnlabz_module_qualification_result_t *qualification
);


const stnlabz_module_inventory_record_t *
stnlabz_module_inventory_find(
    const stnlabz_module_inventory_t *inventory,
    const stnlabz_module_descriptor_t *descriptor
);


const char *stnlabz_module_inventory_result_string(
    stnlabz_module_inventory_result_t result
);


#endif