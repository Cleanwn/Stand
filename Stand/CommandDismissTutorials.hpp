#pragma once

#include "CommandAction.hpp"

#include "Gui.hpp"
#include "Tutorial.hpp"

namespace Stand
{
	class CommandDismissTutorials : public CommandAction
	{
	public:
		explicit CommandDismissTutorials(CommandList* const parent)
			: CommandAction(parent, LOC("DISM"), CMDNAMES_OBF("dismisstutorial", "endtutorial", "skiptutorial"))
		{
		}

		void onClick(Click& click) final
		{
			// Reset any visual changes tutorial did to menu
			Tutorial::fastForward();

			// Dismiss any post-tutorial instructions
			g_gui.user_understands_sliders = true;
			g_gui.user_understands_commands = true;
			g_gui.user_understands_navigation = true;
			g_gui.user_understands_context_menu = true;
			//g_gui.user_understands_command_chaining = true;
			g_gui.setProfilesTutorialDone(); // & save the profile
		}
	};
}