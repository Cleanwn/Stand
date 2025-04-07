#pragma once

#include "fwddecl.hpp"

#include <vector>

namespace Stand
{
	struct PlayerExcludes
	{
		enum : uint8_t
		{
			SHOW_ORG_MEMBERS = 1 << 0,
			SHOW_MODDER = 1 << 1,
			SHOW_LIKELY_MODDER = 1 << 2
		};

		bool exclude_friends = false;
		bool exclude_crew_members = false;
		bool exclude_stand_users = false;
		bool exclude_org_members = false;
		bool exclude_modders = false;
		bool exclude_likely_modders = false;

		void populateList(CommandList* list, const std::vector<CommandName>& command_names_prefix = {}, uint8_t flags = 0);

		[[nodiscard]] std::vector<AbstractPlayer> getNonExcludedPlayers(bool exclude_self) const;
		[[nodiscard]] bool isExcluded(AbstractPlayer p) const;
	};
}
