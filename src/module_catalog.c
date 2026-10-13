/*
 * STN-LABZ
 * Module ABI
 *
 * module_catalog.c
 *
 * Static Core catalog of known module implementations.
 *
 * Filesystem discovery does not create executable
 * trust. A discovered module ID must map to a
 * descriptor already registered in this catalog.
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "module_catalog.h"


void stnlabz_module_catalog_init(
    stnlabz_module_catalog_t *catalog
)
{
    if (
        catalog == NULL
    )
    {
        return;
    }


    memset(
        catalog,
        0,
        sizeof(*catalog)
    );
}


void stnlabz_module_catalog_destroy(stnlabz_module_catalog_t *catalog)
{
    if (!catalog) return;
    free(catalog->entries);
    memset(catalog, 0, sizeof(*catalog));
}

stnlabz_module_result_t stnlabz_module_catalog_register(
    stnlabz_module_catalog_t *catalog,
    const stnlabz_module_descriptor_t *descriptor
)
{
    size_t index;


    if (
        catalog == NULL ||
        descriptor == NULL ||
        descriptor->id[0] == '\0' ||
        descriptor->name[0] == '\0' ||
        descriptor->qualify == NULL
    )
    {
        return
            STNLABZ_MODULE_ERR_INVALID_ARGUMENT;
    }


    for (
        index = 0;
        index < catalog->count;
        ++index
    )
    {
        if (
            strcmp(
                catalog
                    ->entries[index]
                    ->id,
                descriptor->id
            ) == 0
        )
        {
            return
                STNLABZ_MODULE_ERR_DUPLICATE;
        }
    }


    if (catalog->count == catalog->capacity)
    {
        size_t cap;
        const stnlabz_module_descriptor_t **p;
        if (catalog->capacity > SIZE_MAX / 2)
            return STNLABZ_MODULE_ERR_REGISTRY_FULL;
        cap = catalog->capacity ? catalog->capacity * 2 : 16;
        if (cap > SIZE_MAX / sizeof(*p))
            return STNLABZ_MODULE_ERR_REGISTRY_FULL;
        p = realloc(catalog->entries, cap * sizeof(*p));
        if (!p) return STNLABZ_MODULE_ERR_REGISTRY_FULL;
        catalog->entries = p;
        catalog->capacity = cap;
    }


    catalog->entries[
        catalog->count
    ] =
        descriptor;


    catalog->count++;


    return
        STNLABZ_MODULE_OK;
}


const stnlabz_module_descriptor_t *stnlabz_module_catalog_find(
    const stnlabz_module_catalog_t *catalog,
    const char *module_id
)
{
    size_t index;


    if (
        catalog == NULL ||
        module_id == NULL
    )
    {
        return NULL;
    }


    for (
        index = 0;
        index < catalog->count;
        ++index
    )
    {
        if (
            strcmp(
                catalog
                    ->entries[index]
                    ->id,
                module_id
            ) == 0
        )
        {
            return
                catalog->entries[index];
        }
    }


    return NULL;
}