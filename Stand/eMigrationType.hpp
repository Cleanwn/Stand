#pragma once

#include "gta_fwddecl.hpp"

namespace rage
{
  enum eMigrationType : int32_t
  {
    MIGRATE_PROXIMITY = 0x0,
    MIGRATE_OUT_OF_SCOPE = 0x1,
    MIGRATE_SCRIPT = 0x2,
    MIGRATE_FORCED = 0x3,
    MIGRATE_REASSIGNMENT = 0x4,
    MIGRATE_FROZEN_PED = 0x5,
    NUM_MIGRATION_TYPES = 0x6,
  };
}