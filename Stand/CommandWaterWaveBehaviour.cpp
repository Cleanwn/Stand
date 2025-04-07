#include "CommandWaterWaveBehaviour.hpp"

#include "AbstractEntity.hpp"
#include "regular_event.hpp"
#include "natives.hpp"

namespace Stand
{
	CommandWaterWaveBehaviour::CommandWaterWaveBehaviour(CommandList* const parent)
		: CommandListSelect(parent, LOC("WVEBHV"), CMDNAMES("wavebehaviour", "wavebehavior", "waterbehaviour", "waterbehavior"), LOC("GEN_H_LOC"), {
			{0, LOC("NRML"), CMDNAMES("normal")},
			{1, LOC("SMTH"), CMDNAMES("smooth")},
			{2, LOC("MGNT"), CMDNAMES("magnet")},
		}, 0)
	{
	}

	static void water_magnet_on_tick()
	{
		const v3 pos = g_player_ped.getPos();
		for (float x = pos.x - 10.0f; x <= (pos.x + 10.0f); x += 1.0f)
		{
			for (float y = pos.y - 10.0f; y <= (pos.y + 10.0f); y += 1.0f)
			{
				if (MISC::GET_DISTANCE_BETWEEN_COORDS(pos.x, pos.y, 0, x, y, 0, false) <= 5.0f)
				{
					WATER::MODIFY_WATER(x, y, 1.0f, 1.0f);
				}
			}
		}
	}

	void CommandWaterWaveBehaviour::onChange(Click& click, long long prev_value)
	{
		if (prev_value == 2)
		{
			reScriptTickEvent::unregisterHandler(&water_magnet_on_tick);
		}
		switch (value)
		{
		case 1:
			registerScriptTickEventHandler(click, [=]()
			{
				if (value == 3)
				{
					WATER::SET_DEEP_OCEAN_SCALER(0.0f);
					return true;
				}
				else
				{
					WATER::RESET_DEEP_OCEAN_SCALER();
					return false;
				}
			});
			break;

		case 2:
			reScriptTickEvent::registerHandler(&water_magnet_on_tick);
			break;
		}
	}
}
