#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandKeepCursorBack : public CommandToggle
	{
	public:
		explicit CommandKeepCursorBack(CommandList* const parent)
			: CommandToggle(parent, LOC("LSTS"), { CMDNAME("listskeepcursor"), CMDNAME("listkeepscursor") }, LOC("KPCRSRS_H"))
		{
		}

		void onChange(Click& click) final
		{
			g_gui.reset_cursor_on_back = !m_on;
		}
	};
}
