#pragma once

#include "CommandStatSlider.hpp"

namespace Stand
{
	class CommandStatFloatSlider : public CommandStatSlider<CommandSliderFloat>
	{
	public:
		using CommandStatSlider::CommandStatSlider;

	protected:
		void setStat(Hash stat) final
		{
			STATS::STAT_SET_FLOAT(stat, getFloatValue(), TRUE);
		}

		void getStat(Hash stat, int* value) final
		{
			float v = 0.0f;
			STATS::STAT_GET_FLOAT(stat, &v, -1);
			*value = (int)(getPrecisionScalar() * v);
		}
	};
}