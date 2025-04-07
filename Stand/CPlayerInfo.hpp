#pragma once

#include "struct_base.hpp"

#include "gta_extensible.hpp"
#include "rlGamerInfo.hpp"

#pragma pack(push, 1)
enum eWantedLevel : int32_t
{
	WANTED_CLEAN = 0,
	WANTED_LEVEL1,
	WANTED_LEVEL2,
	WANTED_LEVEL3,
	WANTED_LEVEL4,
	WANTED_LEVEL5,

	WANTED_LEVEL_LAST
};

class CWanted
{
  PAD(0, 0x20) int32_t m_nWantedLevel;
  /* 0x24 */ int32_t m_nWantedLevelBeforeParole;
  /* 0x28 */ uint32_t m_LastTimeWantedLevelChanged;
  /* 0x2C */ uint32_t m_TimeWhenNewWantedLevelTakesEffect;
  /* 0x30 */ int32_t m_TimeOutsideCircle;
  /* 0x34 */ int32_t m_TimeOfParole;
  /* 0x38 */ float m_fMultiplier;
  /* 0x3C */ float m_fDifficulty;
  /* 0x40 */ float m_fTimeSinceLastDifficultyUpdate;
  /* 0x44 */ uint8_t m_nMaxCopCarsInPursuit;
  /* 0x45 */ uint8_t m_CausedByPlayerPhysicalIndex;
  PAD(0x45 + 1, 0x74) uint32_t m_iTimeSearchLastRefocused;
  /* 0x78 */ uint32_t m_iTimeLastSpotted;
  /* 0x7C */ uint32_t m_iTimeFirstSpotted;
  /* 0x80 */ uint32_t m_iTimeHiddenEvasionStarted;
  /* 0x84 */ uint32_t m_iTimeHiddenWhenEvasionReset;
  /* 0x88 */ uint32_t m_iTimeInitialSearchAreaWillTimeOut;
  /* 0x8C */ uint32_t m_uLastEvasionTime;
  /* 0x90 */ float m_fTimeEvading;
  /* 0x94 */ uint16_t m_nChanceOnRoadBlock;
  /* 0x96 */ bool m_PoliceBackOff;
  /* 0x97 */ bool m_PoliceBackOffGarage;
  /* 0x98 */ bool m_EverybodyBackOff;
  /* 0x99 */ bool m_AllRandomsFlee;
  /* 0x9A */ bool m_AllNeutralRandomsFlee;
  /* 0x9B */ bool m_AllRandomsOnlyAttackWithGuns;
  /* 0x9C */ bool m_IgnoreLowPriorityShockingEvents;
  /* 0x9D */ bool m_DontDispatchCopsForThisPlayer;
  /* 0x9E */ bool m_MaintainFlashingStarAfterOffence;
  /* 0x9F */ bool m_CenterWantedPosition;
  /* 0xA0 */ bool m_SuppressLosingWantedLevelIfHiddenThisFrame;
  /* 0xA1 */ bool m_AllowEvasionHUDIfDisablingHiddenEvasionThisFrame;
  /* 0xA2 */ bool m_HasLeftInitialSearchArea;
  /* 0xA3 */ bool m_DisableCallPoliceThisFrame;
  /* 0xA4 */ bool m_LawPedCanAttackNonWantedPlayer;
  /* 0xA5 */ float m_fCurrentChaseTimeCounter;
  /* 0xA9 */ float m_fCurrentChaseTimeCounterMaxStar;
  /* 0xAD */ int32_t m_iTimeBeforeAllowReporting;
  /* 0xB1 */ bool m_bTimeCounting;
  /* 0xB2 */ bool m_bTimeCountingMaxStar;
  /* 0xB3 */ bool m_bIsOutsideCircle;
  /* 0xB4 */ eWantedLevel m_WantedLevelOld;
  /* 0xB8 */ eWantedLevel m_WantedLevel;
  /* 0xBC */ eWantedLevel m_WantedLevelBeforeParole;
  /* 0xC0 */ int32_t m_nNewWantedLevel;
  /* 0xC4 */ uint32_t m_WantedLevelCharId;
};
static_assert(sizeof(CWanted) == 0xC8);

// within CPlayerInfo::SetPlayerPed: 81 E1 FF BF E0 FF
// within CPlayerInfo::SetInitialState: 80 A1 ? ? ? ? 3F 80 A1 ? ? ? ? 1E
class CPlayerInfo : public rage::fwExtensibleBase
{
public:
	/* 0x20 */ rage::rlGamerInfo gamer_info; // correct 3095
	PAD(0x20 + sizeof(rage::rlGamerInfo), 0x1C8) float m_swim_speed;
	PAD(0x1C8 + 4, 0x230) int32_t game_state; // for possible values, see FORCE_GAME_STATE_PLAYING
	PAD(0x230 + 4, 0x240) CPed* m_ped;
	PAD(0x240 + 8, 0x270) uint32_t m_frame_flags; // CPlayerResetFlags
	PAD(0x270 + 4, 0x830) CWanted m_Wanted;
	PAD(0x830 + sizeof(CWanted), 0xD50) float m_run_speed;
	/* 0xD54 */ float stamina; // max: 100.0f
	PAD(0xD54 + 4, 0xD6C) float m_fWeaponDamageModifier;
	float m_fWeaponDefenseModifier;
	float m_fWeaponMinigunDefenseModifier;
	float m_fMeleeWeaponDamageModifier;
	float m_fMeleeUnarmedDamageModifier;
	float m_fMeleeWeaponDefenseModifier;
	float m_fMeleeWeaponForceModifier;
	float m_fVehicleDamageModifier;
	float m_fVehicleDefenseModifier;
	PAD(0xD8C + 4, 0xFF8) float m_fFallHeightOverride;
	PAD(0x1008 + 4, 0x1060) uint64_t flags; // 1 = inside moving train, 2 = allow control inside moving train
	/* 0x1068 */ CTrain* train_inside_of;
};
static_assert(offsetof(CPlayerInfo, m_ped) == 0x240); // 4C 8B 81 ? ? ? ? 41 8B 80 ? ? ? ? C1 E8 1E A8 01
static_assert(offsetof(CPlayerInfo, m_swim_speed) == 0x1C8);
static_assert(offsetof(CPlayerInfo, m_frame_flags) == 0x270);
static_assert(offsetof(CPlayerInfo, m_Wanted) == 0x830); // CPlayerInfo::SetPlayerPed
static_assert(offsetof(CPlayerInfo, m_fWeaponDamageModifier) == 0xD6C); // CPlayerInfo::SetInitialState
#pragma pack(pop)
