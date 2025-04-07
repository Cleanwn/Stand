#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandBypassPiMenuGreyedOut : public CommandToggle
	{
	public:
		explicit CommandBypassPiMenuGreyedOut(CommandList* parent)
			: CommandToggle(parent, LOC("ALLPICLK"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAMES("pimenugrey", "pimenugray")), LOC("ALLPICLK_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (is_session_started())
				{
					ScriptGlobal(GLOBAL_AM_PI_MENU_OPTION_ENABLED).set<BOOL>(TRUE);
				}
			});
		}
	};
}
