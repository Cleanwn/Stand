#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandNoExtraClear : public CommandToggle
	{
	public:
		explicit CommandNoExtraClear(CommandList* const parent)
			: CommandToggle(parent, LIT_OBF("Don't Clear Extras"), CMDNAMES(), NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_TEMPORARY)
		{
		}

		void onChange(Click& click) final
		{
			g_gui.no_extra_clear = m_on;
		}
	};
}
