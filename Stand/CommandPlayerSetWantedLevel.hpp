#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandSlider.hpp"

namespace Stand
{
	class CommandPlayerSetWantedLevel : public CommandOnPlayer<CommandSlider>
	{
	public:
		explicit CommandPlayerSetWantedLevel(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("WNTD"), CMDNAMES_OBF("pwanted", "pwantedlevel"), NOLABEL, 0, 5, 0, 1, CMDFLAGS_SLIDER, COMMANDPERM_RUDE, true)
		{
			PC_PTR->registerCommand(this);
		}

		void onClick(Click& click) final
		{
			ensureYieldableScriptThread(click, [this]
			{
				for (const auto& p : PP_PTR->getPlayers())
				{
					p.setWantedLevel(this->value);
					Script::current()->yield(300); // needed for this to work on all players
				}
			});
		}
	};
}