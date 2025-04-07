#pragma once

#include <cstdint>

#include <macros.hpp>

class PlayerCard
{
	enum Status : uint8_t
	{
		ONLINE_IN_SESSION = 0x10,
		ONLINE_MP = 0x08,
		ONLINE_SP = 0x04,
		ONLINE_OTHER = 0x02,
		OFFLINE = 0x01,
	};

	PAD(0x00, 0x02) char name[16];
	PAD(0x12, 0x14) Status status; // treated like a bitflag. defaults to offline.
	PAD(0x15, 0x28)
};
static_assert(sizeof(PlayerCard) == 0x28);

class CBasePlayerCardManager
{
	PAD(0x00 + 0, 0x10) uint16_t capacity;
	PAD(0x10 + 2, 0x18) PlayerCard* cards;
};
