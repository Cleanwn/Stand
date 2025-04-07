#pragma once

#include "CommandSliderDynamic.hpp"

#include "AbstractEntity.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandWindowTint : public CommandSliderDynamic
	{
	public:
		explicit CommandWindowTint(CommandList* const parent)
			: CommandSliderDynamic(parent, LOC("WINTINT"), CMDNAMES("windowtint"), NOLABEL, 0, 0, 0)
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
						VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, value);
					}
				});
			}
		}

		void onTickInGameViewport() final
		{
			auto veh = Util::getVehicle();
			if (veh.isValid())
			{
				setMaxValue(6);
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				setValue(click, VEHICLE::GET_VEHICLE_WINDOW_TINT(veh));
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
