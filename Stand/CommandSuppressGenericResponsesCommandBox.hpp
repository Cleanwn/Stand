#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandSuppressGenericResponsesCommandBox : public CommandToggle
	{
	public:
		explicit CommandSuppressGenericResponsesCommandBox(CommandList* const parent)
			: CommandToggle(parent, LOC("B_CB"), CMDNAMES("suppressgenericresponses"), LOC("SUPPGEN_CB_H"))
		{
		}

		void onChange(Click& click) final
		{
			g_gui.suppress_generic_command_box_responses = m_on;
		}
	};
}
