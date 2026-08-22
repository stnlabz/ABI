#ifndef STNLABZ_MODULE_DISCOVERY_H
#define STNLABZ_MODULE_DISCOVERY_H

#include <stddef.h>

#include "module.h"
#include "module_loader.h"
#include "module_registry.h"


typedef struct
{
    size_t directories_examined;

    size_t modules_loaded;

    size_t modules_discovered;

    size_t modules_rejected;

} stnlabz_module_discovery_report_t;


/*
 * Resolve the module directory relative to
 * host executable:
 *
 *     <host executable directory>\modules
 */

stnlabz_module_result_t
stnlabz_module_discovery_get_path(
    char *modules_path,
    size_t modules_path_size
);


/*
 * Scan the module directory, load DLL modules,
 * obtain their descriptors through the approved
 * ABI, and submit valid descriptors to the Core
 * registry.
 */

stnlabz_module_result_t
stnlabz_module_discovery_scan(
    stnlabz_module_registry_t *registry,
    stnlabz_module_loader_t *loader,
    const char *modules_path,
    stnlabz_module_discovery_report_t *report
);


#endif