#pragma once

#include "CommandListPlaces.hpp"

#include "Util.hpp"

namespace Stand
{
	class CommandWaypoint : public CommandListPlaces
	{
	public:
		explicit CommandWaypoint(CommandList* const parent)
			: CommandListPlaces(parent, LOC("WPS"), { CMDNAME("wp"), CMDNAME("waypoint") })
		{
			CommandPhysical::collapse_command_names.emplace(L"wp");
			CommandPhysical::collapse_command_names.emplace(L"waypoint");
		}

		void populate()
		{
			CommandListPlaces::populate(false);
		}

		[[nodiscard]] bool supportsUsageByOthers() const noexcept final
		{
			return false;
		}

		[[nodiscard]] std::vector<CommandName> decorateCommandNames(const std::vector<CommandName>& command_names) const
		{
			return combineCommandNames(std::vector<CommandName>{ CMDNAME("wp"), CMDNAME("waypoint") }, command_names);
		}

		void processPop(Click& click, const PointOfPresence& pop) const final
		{
			Util::setWaypoint(pop.pos);
		}
	};
}
