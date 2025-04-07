#pragma once

#include "CommandToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandCommendToast : public CommandToggle
	{
	public:
		explicit CommandCommendToast(CommandList* const parent)
			: CommandToggle(parent, LOC("CMENDT"), { CMDNAME("commendnotify") }, LOC("CMENDT_H"))
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.notify_on_commendation = m_on;
		}
	};
}
