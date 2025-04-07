#pragma once

#include "evtEvent.hpp"

#include "gta_player.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct evtPlayerJoinEvent : public evtEvent<evtPlayerJoinEvent>
	{
		const compactplayer_t player;
		const bool notify;

		evtPlayerJoinEvent(const compactplayer_t player, const bool notify)
			: player(player), notify(notify)
		{
		}
	};
#pragma pack(pop)
}
