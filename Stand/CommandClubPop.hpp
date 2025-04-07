#pragma once

#include "CommandSlider.hpp"

#include "lang.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandClubPop : public CommandSlider
	{
	public:
		explicit CommandClubPop(CommandList* const parent)
			: CommandSlider(parent, LOC("CLUBPOP"), CMDNAMES("clubpopularity"), NOLABEL, 0, 100, 100, 10, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true)
		{
		}

		void onClick(Click& click) final
		{
			const int char_slot = Util::getCharSlot(click);
			if (char_slot != -1)
			{
				ensureScriptThread(click, [=]
				{
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CLUB_POPULARITY"), value * 10, true);
				});
			}
		}
	};
}
