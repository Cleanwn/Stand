#pragma once

#include "evtEvent.hpp"

#include "gta_player.hpp"

namespace Stand
{
	struct evtHostChangeEvent : public evtEvent<evtHostChangeEvent>
	{
		const compactplayer_t cur;

		explicit evtHostChangeEvent(compactplayer_t cur)
			: cur(cur)
		{
		}
	};
}
