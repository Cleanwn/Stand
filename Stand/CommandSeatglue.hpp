#pragma once

#include "CommandToggle.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandSeatglue : public CommandToggle
	{
	public:
		static inline CommandToggle* instance = nullptr;

		explicit CommandSeatglue(CommandList* const parent)
			: CommandToggle(parent, LOC("SEATGLUE"), CMDNAMES("seatbelt", "seatglue"), LOC("SEATGLUE_H"))
		{
			instance = this;
		}

		~CommandSeatglue()
		{
			if (instance == this)
			{
				instance = nullptr;
			}
		}

		void onChange(Click& click) final
		{
			g_gui.seatglue = m_on;
		}
	};
}
