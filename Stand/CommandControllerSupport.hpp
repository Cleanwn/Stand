#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandControllerSupport : public CommandToggle
	{
	public:
		explicit CommandControllerSupport(CommandList* const parent)
			: CommandToggle(parent, LOC("CTRLLRSPPRT"), CMDNAMES("controller"), NOLABEL, true)
		{
		}

		void onChange(Click& click) final
		{
			g_gui.controller_support = m_on;
		}
	};
}
