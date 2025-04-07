#pragma once

#include "CommandToggle.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandDisableRestrictedAreas : public CommandToggle
	{
	public:
		explicit CommandDisableRestrictedAreas(CommandList* const parent)
			: CommandToggle(parent, LOC("DSBLRESTRAR"), combineCommandNames(CMDNAMES_OBF("no", "block", "disable"), CMDNAMES_OBF("restrictedareas")), LOC("DSBLRESTRAR_H"))
		{
		}

		void onChange(Click& click) final
		{
			g_hooking.disable_restricted_areas = m_on;
		}
	};
}