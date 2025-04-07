#pragma once

#include "CommandPlayerAction.hpp"

#include "AbstractEntity.hpp"
#include "PlaceObjective.hpp"
#include "TpUtil.hpp"

namespace Stand
{
	class CommandPlayerTpToMyObjective : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerTpToMyObjective(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYSMMNOBJ"), CMDNAMES_OBF("summonobj", "summonobjective", "objectivesummon", "objsummon"))
		{
		}

		void onClick(Click& click) final
		{
			PlaceObjective objective;

			if (auto option = objective.getClosestPop(click); option.has_value())
			{
				const auto pos = option.value().pos;

				queueJob([this, pos]
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						if (p == g_player)
						{
							TpUtil::teleport(pos);
						}
						else
						{
							p.fiberTeleport(pos);
						}
					}
				});
			}
		}
	};
}