#pragma once

#include "CommandSliderScriptTick.hpp"
#include "CommandSlider.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandClockSpeed : public CommandSliderScriptTick<CommandSlider>
	{
	public:
		explicit CommandClockSpeed(CommandList* parent)
			: CommandSliderScriptTick(parent, LOC("SPD"), CMDNAMES("clockspeed"), NOLABEL, 1, 3999, 2000, 50)
		{
		}

		Label getActivationName() const final
		{
			return getActivationNameImplCombineWithParent();
		}

		void onScriptTick() final
		{
			if (GET_MILLIS_SINCE(parent->as<CommandClock>()->last_sync) > 5000)
			{
				NETWORK::NETWORK_OVERRIDE_CLOCK_RATE((4000 - value));
			}
		}
	};
}
