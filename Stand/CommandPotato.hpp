#pragma once

#include "CommandToggle.hpp"

#include "Camgr.hpp"

namespace Stand
{
	class CommandPotato : public CommandToggle
	{
	public:
		explicit CommandPotato(CommandList* const parent)
			: CommandToggle(parent, LOC("POTAT"), CMDNAMES("potatomode"), LOC("POTAT_H"))
		{
		}

		void onEnable(Click& click) final
		{
			Camgr::potato_mode = true;
		}

		void onDisable(Click& click) final
		{
			Camgr::potato_mode = false;
			if (Camgr::shouldClearFocus())
			{
				ensureScriptThread(click, []
				{
					STREAMING::CLEAR_FOCUS();
				});
			}
		}
	};
}
