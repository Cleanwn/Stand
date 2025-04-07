#pragma once

#include "evtEvent.hpp"

#include "gta_player.hpp"

namespace Stand
{
	struct evtFlowEventDoneEvent : public evtEvent<evtFlowEventDoneEvent>
	{
		const compactplayer_t player;
		const flowevent_t type;
		const Label name;
		const std::string extra_data;

		evtFlowEventDoneEvent(const compactplayer_t player, const flowevent_t type, const Label& name, const std::string& extra_data)
			: player(player), type(type), name(name), extra_data(extra_data)
		{
		}
	};
}
