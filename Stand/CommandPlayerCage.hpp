#pragma once

#include "CommandTextslider.hpp"
#include "CommandOnPlayer.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandPlayerCage : public CommandOnPlayer<CommandTextslider>
	{
	public:
		explicit CommandPlayerCage(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("PLYCGE"), { CMDNAME("cage") }, NOLABEL, {
				{ ATSTRINGHASH("prop_rub_cage01a"), LOC("PLYCAGE_TINY") },
				{ ATSTRINGHASH("prop_gold_cont_01"), LOC("PLYCAGE_GOLD") },
				{ ATSTRINGHASH("prop_fnclink_03e"), LOC("PLYCAGE_FNCE") },
				{ ATSTRINGHASH("stt_prop_stunt_tube_s"), LOC("PLYCAGE_STNT") }
			}, CMDFLAGS_ACTION_SLIDER, COMMANDPERM_AGGRESSIVE)
		{
			PC_PTR->registerCommandNames(this);
		}

		void onClick(Click& click) final
		{
			for (const auto& p : PP_PTR->getPlayers())
			{
				if (auto ped = p.getPed(); ped.isValid())
				{
					ped.cage((Hash)this->value);
				}
			}
		}
	};
}
