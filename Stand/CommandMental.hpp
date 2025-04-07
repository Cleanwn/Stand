#pragma once

#include "CommandSlider.hpp"

#include "AbstractPlayer.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandMental : public CommandSlider
	{
	public:
		explicit CommandMental(CommandList* const parent)
			: CommandSlider(parent, LOC("MNTL"), CMDNAMES("mentalstate"), NOLABEL, 0, 100, 0, 10, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true)
		{
		}

		void onClick(Click& click) final
		{
			const int char_slot = Util::getCharSlot(click);
			if (char_slot != -1)
			{
				const auto fvalue = (float)value;
				ensureScriptThread(click, [char_slot, fvalue]
				{
					if (is_session_started())
					{
						*g_player.mentalState() = fvalue;
					}
					STATS::STAT_SET_FLOAT(CHAR_STAT(char_slot, "PLAYER_MENTAL_STATE"), fvalue, true);
				});
				click.setResponse(LIT(LANG_FMT("MNTL_T", value)));
			}
		}
	};
}
