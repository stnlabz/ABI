# ABI
Cross APP and Platform Modules ABI

STN-LABZ Module ABI 1.4
Lifecycle bump for deterministic hot replacement.

ABI change:
  1.3 -> 1.4

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
