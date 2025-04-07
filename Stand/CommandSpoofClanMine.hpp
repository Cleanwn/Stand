#pragma once

#include "CommandListAction.hpp"

#include "CommandListActionItem.hpp"

namespace Stand
{
	class CommandSpoofClanMine : public CommandListAction
	{
	public:
		explicit CommandSpoofClanMine(CommandList* const parent)
			: CommandListAction(parent, LOC("SPFCRWMINE"), {}, NOLABEL, {}, 0, CMDFLAGS_LIST_ACTION | CMDFLAG_FEATURELIST_SKIP)
		{
		}

		void onClick(Click& click) final
		{
			std::vector<CommandListActionItemData> options{};
			for (long long i = 0; i < pointers::clan_client->num_memberships; ++i)
			{
				options.emplace_back(i, CommandSpoofClan::getClanLabel(pointers::clan_client->memberships[i].clan.tag, pointers::clan_client->memberships[i].clan.name, pointers::clan_client->memberships[i].clan.alt_badge));
			}
			setOptions(std::move(options));
			CommandListAction::onClick(click);
		}
		
		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			CommandListAction::onItemClick(click, item);
			memcpy(&g_hooking.spoofed_clan_membership, &pointers::clan_client->memberships[item->value], sizeof(rage::rlClanMembershipData));
			g_hooking.initSpoofedClan();
			parent->as<CommandSpoofClan>()->setValuesFromHooking(click);
		}
	};
}
