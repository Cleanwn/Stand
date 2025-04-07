#pragma once

#include "CommandSliderScriptTick.hpp"
#include "CommandSliderFloat.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandWaterStrengthOverride : public CommandSliderScriptTick<CommandSliderFloat>
	{
	public:
		explicit CommandWaterStrengthOverride(CommandList* parent)
			: CommandSliderScriptTick(parent, LOC("STROVR"), { CMDNAME("waterstrength") }, NOLABEL, 0, 100000, 0, 10)
		{
		}

		Label getActivationName() const final
		{
			return getActivationNameImplCombineWithParent();
		}

		void onScriptTick() final
		{
			MISC::WATER_OVERRIDE_SET_STRENGTH(getFloatValue());
		}
	};
}
