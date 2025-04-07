#pragma once

#include "CommandPlayerAction.hpp"

#include "CollectablesGiver.hpp"
#include "PlayerProviderSingle.hpp"
#include "Script.hpp"

namespace Stand
{
	class CommandPlayerGiveCollectibles : public CommandPlayerAction
	{
	private:
		const int type;

	public:
		explicit CommandPlayerGiveCollectibles(CommandList* const parent, int type, Label&& name)
			: CommandPlayerAction(parent, std::move(name), (type == -1 ? CMDNAMES("givecollectibles", "collectibles", "givecollectable", "collectables") : CMDNAMES()), NOLABEL, COMMANDPERM_NEUTRAL), type(type)
		{
		}

		void giveAllCollectiblesWithType(int type)
		{
			playerbitset_t targets = 0;
			for (const auto& p : PP_PTR->getPlayers())
			{
				targets |= (1 << p);
			}
			CollectablesGiver::giveAllCollectiblesWithType(targets, type);
		}

		void onClick(Click& click) final
		{
			if (!click.inOnline())
			{
				return;
			}
			ensureYieldableScriptThread(click, [this, wr{ getWeakRef() }]
			{
				if (type == -1)
				{
					for (const auto& type : CollectablesGiver::all_types)
					{
						if (!wr.isValid())
						{
							break;
						}
						giveAllCollectiblesWithType(type);
						Script::current()->yield(200);
					}
				}
				else
				{
					giveAllCollectiblesWithType(type);
				}
			});
		}
	};
}
