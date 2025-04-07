#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandNorthYanktonMap : public CommandToggle
	{
	public:
		explicit CommandNorthYanktonMap(CommandList* const parent)
			: CommandToggle(parent, LOC("YANKMAPRDR"), { CMDNAME("yankmap"), CMDNAME("northyanktonmap") })
		{
		}

		void onChange(Click& click) final
		{
			ensureScriptThread(click, [=]
			{
				HUD::SET_MINIMAP_IN_PROLOGUE(m_on);
			});
		}
	};
}
