#pragma once

#include "gta_net_object_mgr.hpp"

#include "rlclancommon.hpp"
#include "CPedHeadBlendData.hpp"

#pragma pack(push, 1)
class CPlayerCreationDataNode : public CProjectBaseSyncDataNode
{
public:
	/* 0xC0 */ hash_t model;
};

class CPlayerSectorPosNode : public CSyncDataNodeFrequent
{
public:
	/* 0xC0 */ rage::Vector3 pos;
};

class CPlayerCameraDataNode : public CSyncDataNodeFrequent
{
public:
	/* 0x0C0 */ rage::Vector3 absolute_pos;
	PAD(0x0D0, 0x0F0) rage::Vector3 relative_pos;
	/* 0x100 */ float rotUpDown; // 0.0 to pi
	/* 0x104 */ float rotLeftRight; // 0.0 to tau
	PAD(0x108, 0x10A) bool position_is_absolute;
	PAD(0x10B, 0x112) bool first_person;
};
static_assert(sizeof(CPlayerCameraDataNode) == 0x112 + 1);

class CPlayerGamerDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0x0C0 */ rage::rlClanMembershipData clan_memberhsip;
	PAD(0x0C0 + sizeof(rage::rlClanMembershipData), 0x193) bool has_started_transition;
	/* 0x194 */ bool has_transition_session;
	/* 0x195 */ char transition_session_info[169];
	PAD(0x212, 0x214) uint32_t flags;
	PAD(0x214 + 4, 0x22C) uint32_t account_id;
};
//static_assert(sizeof(CPlayerGamerDataNode) == 0x22C + 4);
static_assert(offsetof(CPlayerGamerDataNode, account_id) == 0x258);

// CNetObjPlayer::GetPlayerGameStateData: B8 02 00 00 00 83 C0 FE 41
class CPlayerGameStateDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0x0C0 */ uint32_t m_PlayerState;
	/* 0x0C4 */ IPlayerNodeDataAccessor::PlayerGameStateFlags m_GameStateFlags;
	PAD(0x0C4 + sizeof(IPlayerNodeDataAccessor::PlayerGameStateFlags), 0x10C) int32_t m_MaxHealth;
	PAD(0x10C + 4, 0x128) uint16_t spectated_net_id;
	PAD(0x128 + 2, 0x158) bool driving;
	PAD(0x159, 0x18B) bool holding_handbrake;
	PAD(0x18C, 0x194) bool m_bUseExtendedPopulationRange;
	PAD(0x195, 0x1A0) rage::Vector3 m_vExtendedPopulationRangeCenter;
	PAD(0x1A0 + sizeof(rage::Vector3), 0x1B9) bool super_jump;
	PAD(0x1B9 + 1, 0x1C4) float m_VehicleShareMultiplier;
	/* 0x1C8 */ float m_WeaponDamageModifier;
	/* 0x1CC */ float m_MeleeDamageModifier;
	/* 0x1D0 */ float m_MeleeUnarmedDamageModifier;
};
static_assert(offsetof(CPlayerGameStateDataNode, m_GameStateFlags.notDamagedByBullets) == 0x0DA);
static_assert(offsetof(CPlayerGameStateDataNode, super_jump) == 0x1B9); // 3095
static_assert(offsetof(CPlayerGameStateDataNode, m_VehicleShareMultiplier) == 0x1C4); // 3095
static_assert(offsetof(CPlayerGameStateDataNode, m_WeaponDamageModifier) == 0x1C8); // 3095
static_assert(offsetof(CPlayerGameStateDataNode, m_MeleeDamageModifier) == 0x1CC); // 3095
static_assert(offsetof(CPlayerGameStateDataNode, m_MeleeUnarmedDamageModifier) == 0x1D0); // 3095

class CSyncedPlayerSecondaryPartialAnim
{
public:
	uint32_t m_FloatHash0;
	uint32_t m_FloatHash1;
	uint32_t m_FloatHash2;
	uint32_t m_StatePartialHash;
	uint32_t m_MoveNetDefHash;
	float m_Float0;
	float m_Float1;
	float m_Float2;

	uint32_t m_ClipHash;
	uint32_t m_DictHash;
	uint32_t m_Flags;
	uint32_t m_BoneMaskHash;
	uint32_t m_taskSequenceId;
	bool m_Active;
	bool m_SlowBlendDuration;
	bool m_MoVEScripted;
	bool m_Phone;
	bool m_PhoneSecondary;
	bool m_IsBlocked;
};
static_assert(offsetof(CSyncedPlayerSecondaryPartialAnim, m_Phone) == 0x37);

#define NUM_DECORATION_BITFIELDS 60

class CPlayerAppearanceDataNode : public CSyncDataNodeInfrequent
{
public:
	/* 0x0C0 */ uint32_t m_PackedDecorations[NUM_DECORATION_BITFIELDS];
	/* 0x1B0 */ CSyncedPedVarData commons;
	PAD(0x1B0 + sizeof(CSyncedPedVarData), 0x2B0) CPedHeadBlendData head_blend_data;
	PAD(0x2B0 + sizeof(CPedHeadBlendData), 0x3F0) CSyncedPlayerSecondaryPartialAnim m_secondaryPartialAnim;
	PAD(0x3F0 + sizeof(CSyncedPlayerSecondaryPartialAnim), 0x430) hash_t m_NewModelHash;
	/* 0x434 */ hash_t m_VoiceHash;
	/* 0x438 */ uint32_t m_phoneMode;
	PAD(0x438 + 4, 0x444) bool m_HasHeadBlendData;
	PAD(0x444 + 1, 0x470);
};
static_assert(offsetof(CPlayerAppearanceDataNode, commons) == 0x1B0); // 3095: 0x1A0 -> 0x1B0
static_assert(offsetof(CPlayerAppearanceDataNode, head_blend_data) == 0x2B0); // 3095: 0x2A0 -> 0x2B0
static_assert(offsetof(CPlayerAppearanceDataNode, m_secondaryPartialAnim) == 0x3F0);
static_assert(offsetof(CPlayerAppearanceDataNode, m_NewModelHash) == 0x430); // 3095: 0x420 -> 0x430
static_assert(offsetof(CPlayerAppearanceDataNode, m_HasHeadBlendData) == 0x444); // 3095: 0x434 -> 0x444

class CPlayerExtendedGameStateNode : public CSyncDataNodeInfrequent
{
public:
	/* 0xC0 */ float m_fxWaypoint;
	/* 0xC4 */ float m_fyWaypoint;
	/* 0xC8 */ int16_t m_WaypointObjectId;
	/* 0xCA */ bool m_bHasActiveWaypoint;
	/* 0xCB */ bool m_bOwnsWaypoint;
	/* 0xCC */ unsigned int m_WaypointLocalDirtyTimestamp; // correct 3095
};
static_assert(sizeof(CPlayerExtendedGameStateNode) == 0xCC + 4);

class CPlayerSyncTree : public CPedSyncTreeBase
{
	INIT_PAD(CPedSyncTreeBase, 0x3380) CPlayerCreationDataNode creation_data;
	PAD(0x3380 + sizeof(CPlayerCreationDataNode), 0x35D0) CPlayerSectorPosNode sector_position;
	PAD(0x35D0 + sizeof(CPlayerSectorPosNode), 0x36D0) CPlayerCameraDataNode player_camera;
	PAD(0x36D0 + sizeof(CPlayerCameraDataNode), 0x37F0) CPlayerGameStateDataNode player_game_state;
	PAD(0x37F0 + sizeof(CPlayerGameStateDataNode), 0x39F0) CPlayerAppearanceDataNode appearance_data;
	PAD(0x39F0 + sizeof(CPlayerAppearanceDataNode), 0x4420) CPlayerExtendedGameStateNode extended_game_state;

	[[nodiscard]] const char* getNodeName(const rage::netSyncDataNode* node) const;
};
static_assert(offsetof(CPlayerSyncTree, appearance_data) == 0x39F0); // 3095
static_assert(offsetof(CPlayerSyncTree, extended_game_state) == 0x4420); // 3095
#pragma pack(pop)
