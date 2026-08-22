#ifndef STNLABZ_MODULE_CATALOG_H
#define STNLABZ_MODULE_CATALOG_H

#include <stddef.h>

#include "module.h"


#define STNLABZ_MODULE_CATALOG_MAX \
    32


typedef struct
{
    const stnlabz_module_descriptor_t *
        entries[
            STNLABZ_MODULE_CATALOG_MAX
        ];

    size_t count;

} stnlabz_module_catalog_t;


void stnlabz_module_catalog_init(
    stnlabz_module_catalog_t *catalog
);


stnlabz_module_result_t stnlabz_module_catalog_register(
    stnlabz_module_catalog_t *catalog,
    const stnlabz_module_descriptor_t *descriptor
);


const stnlabz_module_descriptor_t *stnlabz_module_catalog_find(
    const stnlabz_module_catalog_t *catalog,
    const char *module_id
);


#endif