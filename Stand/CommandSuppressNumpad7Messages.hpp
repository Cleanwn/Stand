#pragma once

#include "CommandToggle.hpp"

#include "Renderer.hpp"

namespace Stand
{
	class CommandSuppressNumpad7Messages : public CommandToggle
	{
	public:
		explicit CommandSuppressNumpad7Messages(CommandList* parent)
			: CommandToggle(parent, LOC("PRVNUM7SCO"), {}, NOLABEL, ToggleCorrelation::MENU_OPEN)
		{
		}

		void onChange(Click& click) final
		{
			g_renderer.suppress_numpad_7_messages = m_on;
		}
	};
}
