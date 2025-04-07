#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandGrace : public CommandToggle
	{
	public:
		explicit CommandGrace(CommandList* const parent)
			: CommandToggle(parent, LOC("NRGDL"), { CMDNAME("grace"), CMDNAME("gracefulness"), CMDNAME("noragdoll") }, LOC("NRGDL_H"))
		{
		}

		void onEnable(Click& click) final
		{
			++g_gui.grace;
		}

		void onDisable(Click& click) final
		{
			--g_gui.grace;
		}
	};
}
