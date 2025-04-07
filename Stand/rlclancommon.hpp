#pragma once

#include "struct_base.hpp"

#define RL_CLAN_NAME_MAX_CHARS 25

#pragma pack(push, 1)
namespace rage
{
	using rlClanId = int64_t;
	using rlClanMemberId = int64_t;

	struct rlClanDesc
	{
		rlClanId id;
		uint8_t b;
		uint8_t g;
		uint8_t r;
		PAD(0x0B, 0x14) bool alt_badge;
		PAD(0x15, 0x16) char name[RL_CLAN_NAME_MAX_CHARS];
		/* 0x2E */ char tag[5];
		/* 0x34 */ char motto[65];
	};
	static_assert(sizeof(rlClanDesc) == 0x75);

	struct rlClanMembershipData
	{
		/* 0x00 */ rlClanMemberId member_id;
		/* 0x08 */ rlClanDesc clan;
		PAD(0x7D, 0x80) uint64_t unk_2;
		/* 0x88 */ char position[8]; // observed values: Leader, Rank1, Member
		PAD(0x90, 0xB0) bool is_active;
		PAD(0xB1, 0xB8);
	};
	static_assert(sizeof(rlClanMembershipData) == 0xB8);
}
#pragma pack(pop)
