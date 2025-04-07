#pragma once

#include "CommandSliderDynamic.hpp"

#include "ExecCtx.hpp"

namespace Stand
{
	class CommandVehRoofLivery : public CommandSliderDynamic
	{
	public:
		explicit CommandVehRoofLivery(CommandList* const parent)
			: CommandSliderDynamic(parent, LOC("RFTLIV"), CMDNAMES_OBF("rooflivery"), NOLABEL, 0, 0, 0)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [value{ this->value }]
			{
				VEHICLE::SET_VEHICLE_LIVERY2(g_player_veh, value);
			});
		}

		void onTickInGameViewport() final
		{
			if (ExecCtx::get().isScript())
			{
				const auto count = VEHICLE::GET_VEHICLE_LIVERY2_COUNT(g_player_veh);

				if (count == -1)
				{
					setMaxValue(0);
				}
				else
				{
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					setMaxValue(count - 1); // Seems to be +1 for some reason.
					setValueIndicator(click, VEHICLE::GET_VEHICLE_LIVERY2(g_player_veh));
				}
			}

			if (parent->isT<CommandListConcealer>())
			{
				parent->as<CommandListConcealer>()->setVisible(this, CommandCustomsShowNa::instance->m_on || !isNotApplicable());
			}
		}
	};
}