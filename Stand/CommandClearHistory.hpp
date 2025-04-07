#pragma once

#include "CommandAction.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class CommandClearHistory : public CommandAction
	{
	public:
		CommandToggle* keep_noted;
		CommandToggle* keep_tracked;
		CommandToggle* keep_whitelisted;
		CommandToggle* keep_join_reactions;

		explicit CommandClearHistory(CommandList* const parent)
			: CommandAction(parent, LOC("HIST_CLR"))
		{
		}

		void onClick(Click& click) final
		{
			showWarning(click, getGenericIrreversableWarningLabel(), 0, [this](ThreadContext ctx)
			{
				PlayerHistory::eraseAllExcept(keep_noted->m_on, keep_tracked->m_on, keep_whitelisted->m_on, keep_join_reactions->m_on);
			});
		}
	};
}
