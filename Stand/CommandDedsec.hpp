#pragma once

#include "CommandToggleRegularSp.hpp"

#include "AllEntitiesEveryTick.hpp"

namespace Stand
{
	class CommandDedsec : public CommandToggleRegularSp
	{
	public:
		explicit CommandDedsec(CommandList* const parent)
			: CommandToggleRegularSp(parent, LOC("DEDSEC"), { CMDNAME("dedsecmode"), CMDNAME("dedsec"), CMDNAME("watchdogs") })
		{
		}

		void onChange(Click& click) final
		{
			AllEntitiesEveryTick::dedsec_mode = m_on;
			if (click.type != CLICK_BULK && checkPermissions(click))
			{
				AllEntitiesEveryTick::dedsec_tutorial_state = DEDSEC_TUTORIAL_STATE_TARGET;
			}
		}
	};
}
