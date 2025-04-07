#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandBackClose : public CommandToggle
	{
	public:
		explicit CommandBackClose(CommandList* const parent)
			: CommandToggle(parent, LOC("BKCLS"), CMDNAMES("backclose"), LOC("BKCLS_H"))
		{
		}

		void onChange(Click& click) final
		{
			g_gui.back_closes_menu = m_on;
		}
	};
}
