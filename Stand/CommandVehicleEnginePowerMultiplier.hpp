#pragma once

#include "CommandSliderFloat.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandVehicleEnginePowerMultiplier : public CommandSliderFloat
	{
	public:
		explicit CommandVehicleEnginePowerMultiplier(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("PLYENGMULT"), CMDNAMES_OBF("enginepowermult"), NOLABEL, 100, 2000, 100, 50)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			const auto fvalue = getFloatValue();
			if (fvalue == 1.0f)
			{
				update(fvalue);
				return;
			}
			registerScriptTickEventHandler([this, fvalue]
			{
				HANDLER_CHECK(getFloatValue() == fvalue);
				update(fvalue);
				HANDLER_END;
			});
		}

	private:
		void update(const float fvalue)
		{
			if (g_player_veh.isValid()
				&& g_player_veh.isOwnerOfVehicleAndDriver()
				)
			{
				VEHICLE::MODIFY_VEHICLE_TOP_SPEED(g_player_veh, fvalue * 10.0f);
			}
		}
	};
}