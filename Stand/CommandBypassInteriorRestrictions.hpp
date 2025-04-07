#pragma once

#include "CommandToggle.hpp"

#include "tbDeceiveInteriorChecks.hpp"

namespace Stand
{
	class CommandBypassInteriorRestrictions : public CommandToggle
	{
	public:
		explicit CommandBypassInteriorRestrictions(CommandList* parent)
			: CommandToggle(parent, LOC("NOINTCHK"), combineCommandNames(CMDNAMES("no", "block", "disable", "bypass"), CMDNAME("interiorcheck")), LOC("NOINTCHK_H"))
		{
		}

		void onEnable(Click& click)
		{
			g_tb_deceive_interior_checks.enable();
		}

		void onDisable(Click& click)
		{
			g_tb_deceive_interior_checks.disable();
		}
	};
}
