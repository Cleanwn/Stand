#pragma once

#include "struct_base.hpp"

#include "rlScAccountInfo.hpp"

#pragma pack(push, 1)
namespace rage
{
	enum class rlRosPrivilegeId : uint8_t
	{
		CREATE_TICKET = 1,
		MULTIPLAYER = 3,
		LEADERBOARD_WRITE = 4,
		UNK_NEEDED_TO_GO_ONLINE = 5,
		BANNED = 7,
		SOCIAL_CLUB = 10,
		DEVELOPER = 12,
		CHEATER = 20,
		EXCLUSIVE_CONTENT_1 = 24,
		EXCLUSIVE_CONTENT_2 = 25,
	};

	struct rlRosCredentials
	{
		/* 0x000 */ char scauth_token[600];
		PAD(0x258, 0x280) int nonce;
		PAD(0x280 + 4, 0x288) int64_t account_id; // "PlayerAccountId"
		PAD(0x288 + 8, 0x298) rlScAccountInfo account;
		PAD(0x298 + sizeof(rlScAccountInfo), 0x430) uint32_t privileges;

		[[nodiscard]] bool HasPrivilege(rlRosPrivilegeId idx) const noexcept
		{
			return privileges & (1 << (uint8_t)idx);
		}

		void grantPrivilege(rlRosPrivilegeId idx) noexcept
		{
			privileges |= (1 << (uint8_t)idx);
		}

		void revokePrivilege(rlRosPrivilegeId idx) noexcept
		{
			privileges &= ~(1 << (uint8_t)idx);
		}

		void setPrivilege(rlRosPrivilegeId idx, bool toggle) noexcept
		{
			if (toggle)
			{
				grantPrivilege(idx);
			}
			else
			{
				revokePrivilege(idx);
			}
		}
	};
	static_assert(sizeof(rlRosCredentials) == 0x430 + 4);
}
#pragma pack(pop)
