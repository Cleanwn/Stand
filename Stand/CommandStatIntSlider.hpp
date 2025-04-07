#pragma once

#include "CommandStatSlider.hpp"

namespace Stand
{
	class CommandStatIntSlider : public CommandStatSlider<CommandSlider>
	{
	public:
		using CommandStatSlider::CommandStatSlider;

	protected:
		void setStat(Hash stat) final
		{
			STATS::STAT_SET_INT(stat, this->value, TRUE);
		}

		void getStat(Hash stat, int* value) final
		{
			STATS::STAT_GET_INT(stat, value, -1);
		}
	};
}