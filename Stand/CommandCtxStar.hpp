#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandCtxStar : public CommandToggle
	{
	public:
		explicit CommandCtxStar(CommandList* parent, Command* stars)
			: CommandToggle(
				parent,
				LOC("CTXSTAR"),
				{},
				LIT(LANG_FMT("CTXSTAR_H", stars->getLocalisedAddress())),
				g_gui.starred_commands.data.find(ContextMenu::getTargetPhysical()->getPathConfig()) != g_gui.starred_commands.data.end()
			)
		{
		}

		void onChange(Click& click) final
		{
			if (m_on)
			{
				g_gui.starred_commands.data.emplace(ContextMenu::getTargetPhysical()->getPathConfig(), "Saved");
			}
			else
			{
				g_gui.starred_commands.data.erase(ContextMenu::getTargetPhysical()->getPathConfig());
			}
			g_gui.starred_commands.save();
		}
	};
}
