# ABI
Cross APP and Platform Modules ABI

STN-LABZ Module ABI 1.5
Lifecycle bump for deterministic hot replacement.

ABI change:
  1.3 -> 1.4 -> 1.5

New lifecycle states:
  STOPPED
  UNREGISTERED

Established transitions:

  DISCOVERED
      -> UNVERIFIED
      -> TESTING
      -> QUALIFIED
      -> ACTIVE
      -> STOPPED

  STOPPED
      -> explicit reauthorization
      -> ACTIVE

  STOPPED
      -> UNREGISTERED
      -> registry record removed

Failure paths:
  FAILED
  QUARANTINED

Qualification behavior:
  STOPPED retains qualification evidence for the same loaded revision.
  STOP clears activation authority.
  Reactivation requires explicit authorization.
  A replacement binary/revision re-enters discovery and qualification
  according to the host's normal qualification/inventory rules.

Hot replacement boundary:
  ACTIVE
      -> module stop callback
      -> Core STOPPED transition
      -> Core UNREGISTERED transition
      -> live registry record removed
      -> platform loader may unload old shared library
      -> platform loader loads replacement
      -> new descriptor enters normal discovery/verification/qualification

Important:
  UNREGISTERED is audited before the live registry record is removed.
  Core never reports an unloaded module as ACTIVE.

This package contains the files changed by the lifecycle bump:
  module.h
  module.c
  module_registry.h
  module_registry.c
  abi.h
  abi.c
  main.c

Platform loader/discovery files are intentionally not changed here.
They should consume the new unregister boundary before FreeLibrary/dlclose
during hot replacement.


## Dynamic Core module capacity (host-side layout revision)

The module registry, Linux/Windows loaders, module catalog, and persistent qualification inventory no longer impose 32-entry ceilings. Each grows geometrically with checked `realloc`; exhaustion returns the established FULL/REGISTRY_FULL error without overwriting existing records. Added `stnlabz_module_registry_destroy`, `stnlabz_module_catalog_destroy`, and `stnlabz_module_inventory_destroy`; loader `unload_all` also releases its allocated storage.

**Compatibility notice:** This changes the C **host container struct layouts** (`modules`/`entries`/`records` become pointers and capacities are explicit). Hosts using these structures must rebuild in lockstep against these headers; copying mixed old/new object files is invalid. The module descriptor export `stnlabz_module_get_descriptor`, module descriptor layout, lifecycle callbacks, and qualification rule of ABI 1.5 are unchanged. Existing runtime capacity is limited by process address space and resource availability, not an arbitrary module count. No activation is implied.

After initialization, never call `*_init` a second time on a live allocated container: call the applicable destroy/unload function first. Lifecycle gates still govern removal/unloading, and active module code must be stopped before shared-library unloading.

Validation requires independent Linux and Windows builds, dynamic-registry/loader saturation tests (more than 32), allocation-failure injections, and inventory round-trip tests. Dynamic capacity is not an authority or trust mechanism.
