#pragma once

#include "CommandAction.hpp"

#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandProfileLoad : public CommandAction
	{
	public:
		explicit CommandProfileLoad(CommandList* const parent)
			: CommandAction(parent, LOC("LOAD"), { CMDNAME("load"), CMDNAME("loadprofile") }, LOC("LSTATE_H2"))
		{
			addSuffixToCommandNames(parent->menu_name.getLiteralForCommandName());
		}

		void onClick(Click& click) final
		{
			click.stopInputIfAllowed();
			click.setResponse(LIT(LANG_FMT("LSTATE_T2", parent->menu_name.getLiteralUtf8())));
			FiberPool::queueJob([this]
			{
				// CommandLuaScript::applyCommandStates uses active profile to determinate state,
				// so we'll just switch the active profile to load this one for everything to work as expected.
				parent->as<CommandProfile>()->setActive();

				g_gui.recursivelyApplyActiveState();
			});
		}
	};
}
