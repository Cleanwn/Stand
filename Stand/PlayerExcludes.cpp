#include "PlayerExcludes.hpp"

#include "AbstractPlayer.hpp"
#include "CommandList.hpp"
#include "CommandTogglePointer.hpp"

namespace Stand
{
	void PlayerExcludes::populateList(CommandList* list, const std::vector<CommandName>& command_names_prefix, uint8_t flags)
	{
		list->createChild<CommandTogglePointer>(&exclude_friends, LOC("XCLDF"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME_OBF("excludefriends")));
		list->createChild<CommandTogglePointer>(&exclude_crew_members, LOC("XCLDCM"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME_OBF("excludecrewmembers")));
		list->createChild<CommandTogglePointer>(&exclude_stand_users, LOC("XCLDSU"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME_OBF("excludestandusers")));

		if (flags & SHOW_ORG_MEMBERS)
		{
			list->createChild<CommandTogglePointer>(&exclude_org_members, LOC("XCLDOG"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME_OBF("excludeorgmembers")));
		}

		if (flags & SHOW_MODDER)
		{
			list->createChild<CommandTogglePointer>(&exclude_modders, LOC("XCLDMDR"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME_OBF("excludemodders")));
		}

		if (flags & SHOW_LIKELY_MODDER)
		{
			list->createChild<CommandTogglePointer>(&exclude_likely_modders, LOC("XCLDLMDR"), CommandIssuable::combineCommandNames(command_names_prefix, CMDNAME_OBF("excludelikelymodders")));
		}
	}

	std::vector<AbstractPlayer> PlayerExcludes::getNonExcludedPlayers(bool exclude_self) const
	{
		return AbstractPlayer::listExcept(exclude_self, exclude_friends, exclude_crew_members, exclude_stand_users, exclude_org_members);
	}

	bool PlayerExcludes::isExcluded(AbstractPlayer p) const
	{
		return p.satisfiesAnyOf(exclude_friends, exclude_crew_members, exclude_stand_users, exclude_org_members, exclude_modders, exclude_likely_modders);
	}
}
