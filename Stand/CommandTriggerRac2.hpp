#pragma once

#include "CommandAction.hpp"

#include "AbstractPlayer.hpp"
#include "lang.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandTriggerRac2 : public CommandAction
	{
	public:
		explicit CommandTriggerRac2(CommandList* const parent)
			: CommandAction(parent, LIT("Trigger Rockstar Anti Cheat 2"))
		{
		}

		void onClick(Click& click) final
		{
			showWarning(click, LANG_GET_W("GENWARN"), WARNING_COMMAND_BOUND, [this](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, []
				{
					PLAYER::SET_PLAYER_WANTED_LEVEL(g_player, 0, false);
				});
			});
		}
	};
}
