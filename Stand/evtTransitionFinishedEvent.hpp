#pragma once

#include "evtEvent.hpp"

namespace Stand
{
	struct evtTransitionFinishedEvent : public evtEvent<evtTransitionFinishedEvent>
	{
	};
}
