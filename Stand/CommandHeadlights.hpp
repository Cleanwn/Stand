#pragma once

#include "CommandSliderDynamic.hpp"

namespace Stand
{
	class CommandHeadlights : public CommandSliderDynamic
	{
	public:
		explicit CommandHeadlights(CommandList* const parent)
			: CommandSliderDynamic(parent, LOC("HEADLIT"), CMDNAMES("headlights"), NOLABEL, 0, 0, 0)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			if (!click.isStand())
			{
				ensureScriptThread(click, [this, click]() mutable
				{
					auto veh = Util::getVehicle(click);
					if (veh.isValid())
					{
						veh.setHeadlightsVariation(value);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			if (veh.isValid())
			{
				setMaxValue(14);
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				setValue(click, veh.getHeadlightsVariation());
			}
			else
			{
				setMaxValue(0);
			}

			if (parent->isT<CommandListConcealer>())
			{
				parent->as<CommandListConcealer>()->setVisible(this, CommandCustomsShowNa::instance->m_on || !isNotApplicable());
			}
		}
	};
}
