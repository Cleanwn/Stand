#pragma once

#include "CommandSliderFloat.hpp"

#include "AbstractEntity.hpp"
#include "timescale.hpp"

namespace Stand
{
	class CommandAimTimescale : public CommandSliderFloat
	{
	private:
		CommandSlider* const timescale;

	public:
		explicit CommandAimTimescale(CommandList* const parent, CommandSlider* timescale)
			: CommandSliderFloat(parent, LOC("AIMTMSCL"), { CMDNAME("aimtimescale"), CMDNAME("aimgamespeed"), CMDNAME("aimspeed") }, LOC("AIMTMSCL_H"), 0, 100000, 100, 10), timescale(timescale)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [this]
			{
				const int value = this->value;
				if (value == timescale->value)
				{
					if (g_player_ped.isAiming())
					{
						setTimescale((float)timescale->value / 100.0f);
					}
				}
				else
				{
					registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [this, value]()
					{
						const bool keep_running = (value == this->value && value != timescale->value);
						if (g_player_ped.isAiming() && keep_running)
						{
							setTimescale((float)value / 100.0f);
						}
						else
						{
							setTimescale((float)timescale->value / 100.0f);
						}
						return keep_running;
					});
				}
			});
		}
	};
}
