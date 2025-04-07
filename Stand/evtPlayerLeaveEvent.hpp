#pragma once

#include "evtEvent.hpp"

#include "gta_player.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct evtPlayerLeaveEvent : public evtEvent<evtPlayerLeaveEvent>
	{
		const compactplayer_t player;
		const std::string name;
		const bool notify;

		evtPlayerLeaveEvent(const compactplayer_t player, const std::string& name, const bool notify)
			: player(player), name(name), notify(notify)
		{
		}
	};
#pragma pack(pop)
}
