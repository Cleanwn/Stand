#pragma once

namespace rage
{
	struct playerDataMsg
	{
		virtual ~playerDataMsg() = default;

		/* 0x08 */ uint32_t m_game_version;
		/* 0x0C */ uint32_t m_nat_type;
	};
	static_assert(sizeof(playerDataMsg) == 0x10);
}

class CNetGamePlayerDataMsg : public rage::playerDataMsg
{
public:
	enum
	{
		PLAYER_MSG_FLAG_SPECTATOR = 1 << 0,
		PLAYER_MSG_FLAG_ROCKSTAR_DEV = 1 << 1,
		PLAYER_MSG_FLAG_BOSS = 1 << 2,
		PLAYER_MSG_FLAG_IS_ASYNC = 1 << 3,
		PLAYER_MSG_FLAG_NUM = 4,
	};

	/* 0x10 */ uint32_t unk_added_in_3095;
	/* 0x14 */ uint32_t m_player_type;
	/* 0x18 */ uint32_t m_matchmaking_group;
	/* 0x1C */ uint32_t m_player_flags;
};
static_assert(sizeof(CNetGamePlayerDataMsg) == 0x20);
