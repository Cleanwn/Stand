#pragma once

#include "CommandAction.hpp"

#include "Gui.hpp"
#include "Tutorial.hpp"

namespace Stand
{
	class CommandRepeatTutorial : public CommandAction
	{
	public:
		explicit CommandRepeatTutorial(CommandList* const parent)
			: CommandAction(parent, LOC("RPT"), CMDNAMES_OBF("repeattutorial", "repeatutorial"))
		{
		}

		void onClick(Click& click) final
		{
			g_gui.user_understands_navigation = false;
			g_gui.user_understands_commands = false;
			g_gui.user_understands_sliders = false;
			g_gui.user_understands_context_menu = false;
			g_gui.profiles_tutorial = 0;
			g_gui.user_understands_command_chaining = false;
			Tutorial::start();
		}
	};
}
