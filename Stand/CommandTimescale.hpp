#pragma once

#include "CommandSliderFloat.hpp"

#include "timescale.hpp"

namespace Stand
{
	class CommandTimescale : public CommandSliderFloat
	{
	public:
		CommandSlider* aimtimescale = nullptr;

		explicit CommandTimescale(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("TMSCL"), { CMDNAME("timescale"), CMDNAME("gamespeed") }, LOC("TMSCL_H"), 0, 100000, 100, 25)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [=]
			{
				setTimescale((float)value / 100.0f);
				if (value != 100 && aimtimescale->value == 100)
				{
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					aimtimescale->onChange(click, 0);
				}
			});
		}
	};
}
