#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandSuppressGenericResponsesHotkey : public CommandToggle
	{
	public:
		explicit CommandSuppressGenericResponsesHotkey(CommandList* const parent)
			: CommandToggle(parent, LOC("HOTKEY"))
		{
		}

		void onChange(Click& click) final
		{
			g_gui.suppress_generic_hotkey_responses = m_on;
		}
	};
}
