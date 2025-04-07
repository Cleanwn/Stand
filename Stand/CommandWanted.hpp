#pragma once

#include "CommandSlider.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class CommandWanted : public CommandSlider
	{
	public:
		explicit CommandWanted(CommandList* const parent)
			: CommandSlider(parent, LOC("WNTD"), { CMDNAME("wanted"), CMDNAME("wantedlevel") }, NOLABEL, 0, 5, 0, 1, CMDFLAGS_SLIDER, COMMANDPERM_NEUTRAL, true)
		{
		}

		void onClick(Click& click) final
		{
			if (click.issuedByAndForUser())
			{
				setWantedLevel(click);
			}
		}

		void onChange(Click& click, int prev_value) final
		{
			if (!click.issuedByAndForUser())
			{
				setWantedLevel(click);
				setValueIndicator(click, prev_value);
				return;
			}
			return CommandSlider::onChange(click, prev_value);
		}
		
	private:
		void setWantedLevel(Click& click)
		{
			ensureScriptThread(click, [=]
			{
				click.getEffectiveIssuer().setWantedLevel(value);
			});
		}
	};
}
