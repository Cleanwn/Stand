#pragma once

#include "evtEvent.hpp"

namespace Stand
{
	struct evtPadShakeEvent : public evtEvent<evtPadShakeEvent>
	{
		unsigned int light_duration;
		int light_intensity;
		unsigned int heavy_duration;
		int heavy_intensity;
		int delay_after_this_one;

		evtPadShakeEvent(
			unsigned int light_duration,
			int light_intensity,
			unsigned int heavy_duration,
			int heavy_intensity,
			int delay_after_this_one
		) : light_duration(light_duration),
			light_intensity(light_intensity),
			heavy_duration(heavy_duration),
			heavy_intensity(heavy_intensity),
			delay_after_this_one(delay_after_this_one)
		{
		}
	};
}
