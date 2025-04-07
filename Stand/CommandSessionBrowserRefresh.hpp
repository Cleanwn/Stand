#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandSessionBrowserRefresh : public CommandAction
	{
	public:
		explicit CommandSessionBrowserRefresh(CommandList* const parent)
			: CommandAction(parent, LOC("RFRSH"))
		{
		}

		void onClick(Click& click) final
		{
			if (!parent->as<CommandSessionBrowser>()->request_sent
				|| parent->as<CommandSessionBrowser>()->getNumSessions() != 0
				)
			{
				parent->as<CommandSessionBrowser>()->want_refresh = true;
			}
		}
	};
}
