#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandFinishApartmentHeistSetups : public CommandAction
	{
	public:
		explicit CommandFinishApartmentHeistSetups(CommandList* const parent)
			: CommandAction(parent, LOC("H1_FNSH"), CMDNAMES("setupapartmentheist"), LOC("H1_FNSH_H"))
		{
		}

		void onClick(Click& click) final
		{
			const auto char_slot = Util::getCharSlot(click);
			if (char_slot != -1)
			{
				showWarning(click, LANG_GET_W("H_FNSH_W"), 0, [=](ThreadContext thread_context)
				{
					ensureScriptThread(thread_context, [=]
					{
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "HEIST_PLANNING_STAGE"), -1, true);
						Util::toast(LOC("H1_FNSH_T"));
					});
				});
			}
		}
	};
}
