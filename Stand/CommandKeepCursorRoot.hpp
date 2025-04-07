#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandKeepCursorRoot : public CommandToggle
	{
	public:
		explicit CommandKeepCursorRoot(CommandList* const parent)
			: CommandToggle(parent, LOC("TABS"), { CMDNAME("tabskeepcursor"), CMDNAME("tabkeepscursor") }, LOC("KPCRSRR_H"), true)
		{
		}

		void onChange(Click& click) final
		{
			g_gui.reset_cursor_on_root_navigation = !m_on;
		}
	};
}
