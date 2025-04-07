#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandFinishDoomsdayHeistSetups : public CommandAction
	{
	public:
		explicit CommandFinishDoomsdayHeistSetups(CommandList* const parent)
			: CommandAction(parent, LOC("H2_FNSH"), CMDNAMES("setupdoomsdayheist"), LOC("H2_FNSH_H"))
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
						STATS::STAT_SET_INT(CHAR_STAT(char_slot, "GANGOPS_FLOW_MISSION_PROG"), -1, true);
						Util::toast(LOC("H2_FNSH_T"));
					});
				});
			}
		}
	};
}
