#pragma once

#include "gta_fwddecl.hpp"
#include "gta_player.hpp"

namespace rage
{
	class netPlayerMgrBase
	{
	public:
		virtual ~netPlayerMgrBase() = default; // 0 (0x00)

		bool SendBuffer(const rage::netPlayer* player, const void* data, unsigned int size, unsigned int sendFlags, netSequence* seq, const rage::netPlayer* sender);
	};
}

class CNetworkPlayerMgr : public rage::netPlayerMgrBase
{
public:
	virtual ~CNetworkPlayerMgr() = default;
	virtual void _0x08() = 0;
	virtual void _0x10() = 0;
	virtual void _0x18() = 0;
	virtual void _0x20() = 0;
	virtual void __fastcall removePlayer(CNetGamePlayer*) = 0;

private:
	/* 0x008 */ char pad_0x0008[0x00E8 - 0x0008];
	void* unk_added_in_2944;
public:
	/* 0x0F0 */ CNetGamePlayer* localPlayer;
	PAD(0x0F8, 0x180) __int32 NumPlayers;
	PAD(0x184, 0x188) CNetGamePlayer* Players[32];
	PAD(0x188 + 32 * 8, 0x294) uint32_t m_NumAllActivePlayers;
	PAD(0x298, 0x3A8) CNetGamePlayer* m_AllActivePlayers[32];
	PAD(0x3A8 + 32 * 8, 0x8E0) playerbitset_t player_bitset;
	PAD(0x8E4, 0x3448) size_t num_temp_players;

	[[nodiscard]] compactplayer_t getHostIndex() const;
};
static_assert(offsetof(CNetworkPlayerMgr, localPlayer) == 0xF0);
static_assert(offsetof(CNetworkPlayerMgr, NumPlayers) == 0x180);
static_assert(offsetof(CNetworkPlayerMgr, Players) == 0x188);
static_assert(offsetof(CNetworkPlayerMgr, player_bitset) == 0x8E0);
