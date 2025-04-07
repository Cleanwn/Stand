#pragma once

#include "CommandAction.hpp"

#include "Exceptional.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandProfileSave : public CommandAction
	{
	public:
		explicit CommandProfileSave(CommandList* const parent)
			: CommandAction(parent, LOC("SAVE"), { CMDNAME("save"), CMDNAME("saveprofile") }, LOC("SSTATE_H2"))
		{
			addSuffixToCommandNames(parent->menu_name.getLiteralForCommandName());
		}

		void onClick(Click& click) final
		{
			auto name = parent->as<CommandProfile>()->getConfigName();
			if (g_gui.auto_save_state == name)
			{
				click.setResponse(LOC("AUTOSAVE2_T"));
			}
			else
			{
				click.setResponse(LIT(LANG_FMT("SSTATE_T2", parent->menu_name.getLiteralUtf8())));
				g_gui.saveState(std::move(name));
			}
		}
	};
}
