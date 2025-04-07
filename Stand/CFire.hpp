#pragma once

#include "fwRegdRef.hpp"

enum FireType_e : uint32_t
{
  FIRETYPE_UNDEFINED = 0xFFFFFFFF,
  FIRETYPE_TIMED_MAP = 0x0,
  FIRETYPE_TIMED_PETROL_TRAIL = 0x1,
  FIRETYPE_TIMED_PETROL_POOL = 0x2,
  FIRETYPE_TIMED_PED = 0x3,
  FIRETYPE_TIMED_VEH_WRECKED = 0x4,
  FIRETYPE_TIMED_VEH_WRECKED_2 = 0x5,
  FIRETYPE_TIMED_VEH_WRECKED_3 = 0x6,
  FIRETYPE_TIMED_VEH_GENERIC = 0x7,
  FIRETYPE_TIMED_OBJ_GENERIC = 0x8,
  FIRETYPE_REGD_VEH_WHEEL = 0x9,
  FIRETYPE_REGD_VEH_PETROL_TANK = 0xA,
  FIRETYPE_REGD_FROM_PTFX = 0xB,
  FIRETYPE_MAX = 0xC,
};

struct CFire
{
  /* 0x00 */ rage::Vec3V m_vPosLC1;
  /* 0x10 */ rage::Vec3V m_vNormLC1;
  /* 0x20 */ rage::Vec3V m_vParentPosWld;
  /* 0x30 */ rage::fwRegdRef<CEntity> m_regdEntity;
  /* 0x38 */ int m_boneIndex;
  PAD(0x38 + 4, 0x48) FireType_e m_fireType;
  /* 0x4C */ FireType_e m_parentFireType;
  /* 0x50 */ rage::fwRegdRef<CPed> m_regdCulprit;
  /* 0x58 */ uint32_t m_weaponHash;
  /* 0x5C */ float m_currStrength;
  /* 0x60 */ int16_t m_numGenerations;
  /* 0x62 */ int16_t m_numChildren;
  /* 0x64 */ float m_maxRadius;
  /* 0x68 */ float m_fuelLevel;
  /* 0x6C */ float m_fuelBurnRate;
  /* 0x70 */ uint32_t m_entityTestTime;
  PAD(0x70 + 4, 0x98) float m_totalBurnTime;
  /* 0x9C */ float m_currBurnTime;
  /* 0xA0 */ float m_maxStrength;
  /* 0xA4 */ float m_maxStrengthTime;
  /* 0xA8 */ float m_spreadChanceAccumTime;
};
static_assert(offsetof(CFire, m_spreadChanceAccumTime) == 0xA8);
static_assert(offsetof(CFire, m_entityTestTime) == 0x70);
static_assert(offsetof(CFire, m_boneIndex) == 0x38);
