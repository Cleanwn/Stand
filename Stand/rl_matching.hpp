#pragma once

#include "gta_fwddecl.hpp"
#include "struct_base.hpp"

#include <cstdint>

#include "rl.hpp"

namespace rage
{
	enum eMatchmakingAttributeIds : uint32_t // made up
	{
		MMATTR_DISCRIMINATOR = 0x10000001,
		MMATTR_MM_GROUP_1 = 0x10000013,
		MMATTR_MM_GROUP_2 = 0x10000014, // Player count?
		MMATTR_AIM_TYPE = 0x10000017,
		MMATTR_REGION = 0x10000018,
		MMATTR_ACTIVITY_TYPE = 0x1000001A,
		MMATTR_ACTIVITY_ID = 0x1000001B,
		MMATTR_ACTIVITY_PLAYERS = 0x1000001C,
	};

	struct rlMatchingAttributes
	{
		uint16_t m_GameMode;
		uint16_t m_SessionPurpose;
		unsigned m_Count;
		unsigned m_Ids[RL_MAX_MATCHING_ATTRS];
		char m_Names[RL_MAX_MATCHING_ATTRS][RL_MAX_MATCHING_ATTR_NAME_LENGTH];
		uint32_t m_Vals[RL_MAX_MATCHING_ATTRS];
		uint32_t m_AttrMask;
	};

	struct rlScMatchmakingMatchId
	{
		// example: 98e58b97-b3a7-4e89-9b47-5a696f992a38
		char m_GuidString[38];
		uint8_t m_Guid[16];
	};

	class rlMatchingFilter
	{
	public:
		enum Discriminator // NetworkGameConfig.h
		{
			FREE_AIM = 0x20000, // (1 << 17)

			// This stuff seems to be combined with other stuff & hashed, so it differs
			POOL_NORMAL = (0x37EB << 18),
			POOL_CHEATER = (0x1659 << 18),
			POOL_BADSPORT = (0x328 << 18),
		};

		/* 0x000 */ uint32_t filter_id;
		/* 0x004 */ char filter_name[RL_MAX_MATCHING_FILTER_NAME_LENGTH];
		/* 0x01C */ uint32_t num_conditions;
		/* 0x020 */ uint16_t game_mode; // correct 2824
		/* 0x022 */ uint16_t session_purpose; // correct 2824
		/* 0x024 */ uint32_t attr_ids[RL_MAX_MATCHING_ATTRS];
		/* 0x044 */ char attribute_names[RL_MAX_MATCHING_ATTRS][RL_MAX_MATCHING_ATTR_NAME_LENGTH];
		/* 0x104 */ uint32_t num_attributes;
		/* 0x108 */ uint32_t conditions_lefthand_attr_ids_index[RL_MAX_MATCHING_CONDITIONS];
		/* 0x180 */ uint32_t conditions_operators[RL_MAX_MATCHING_CONDITIONS];
		/* 0x208 */ uint32_t conditions_param_ids[RL_MAX_MATCHING_CONDITIONS];
		/* 0x288 */ uint32_t conditions_righthand_values[RL_MAX_MATCHING_CONDITIONS];
		/* 0x308 */ uint32_t conditions_righthand_values_mask;

		rlMatchingFilter();

		void addCondition(uint32_t lefthand_attr_id, uint32_t oprtor, uint32_t righthand);

		[[nodiscard]] uint32_t getAttrIdIdx(uint32_t attr_id);
	};
	static_assert(sizeof(rlMatchingFilter) == 0x30C);
}

using enum rage::eMatchmakingAttributeIds;
