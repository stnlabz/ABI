STN-LABZ Module ABI 1.3 - orchestration starter files

Files:
  abi.h   Reusable lifecycle orchestration API.
  abi.c   Prepare / authorize+activate / stop / self-test implementation.
  main.c  Minimal standalone self-test entry point.

These files depend on the existing:
  module.h
  module.c
  module_registry.h
  module_registry.c
  module_inventory.h
  module_inventory.c

Platform discovery and loading remain separate.

Important ABI 1.3 limitation:
  The current lifecycle has no DEACTIVATED / STOPPED / UNREGISTERED state.
  stnlabz_module_abi_stop() therefore invokes the module stop callback but
  does not fabricate a registry state transition.

That registry capability should be added before true hot replacement is
implemented.

Namespace:
  All reusable module ABI symbols use the cross-application prefix:
      stnlabz_module_abi_*

  Supporting reusable module types/functions use:
      stnlabz_module_*
      STNLABZ_MODULE_*
