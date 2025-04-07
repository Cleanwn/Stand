#pragma once

#include "CommandToggle.hpp"

#include "gta_entity.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandGod : public CommandToggle
	{
	public:
		explicit CommandGod(CommandList* const parent)
			: CommandToggle(parent, LOC("GOD"), { CMDNAME("godmode"), CMDNAME("immortality") }, LOC("GOD_H"))
		{
		}

		void onEnable(Click& click) final
		{
			g_gui.godmode = true;
		}

		void onDisable(Click& click) final
		{
			g_gui.godmode = false;
			ensureScriptThread(click, []
			{
				g_gui.onGodmodeDisable();
			});
		}
	};
}
