#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandGeoGuessrScout : public CommandToggle
	{
	public:
		explicit CommandGeoGuessrScout(CommandList* const parent)
			: CommandToggle(parent, LOC("GGSR_VIEW"), {}, NOLABEL, true, (CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_CONCEALED)
		{
		}

		void onEnable(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				if (parent->as<CommandGeoGuessr>()->guessed_at == 0)
				{
					parent->as<CommandGeoGuessr>()->startScouting();
				}
			});
		}

		void onDisable(Click& click) final
		{
			ensureScriptThread(click, [this]
			{
				parent->as<CommandGeoGuessr>()->stopScouting();
			});
		}
	};
}
