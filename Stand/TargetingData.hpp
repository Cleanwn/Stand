#pragma once

#include <cstdint>

#include "CommandDivider.hpp"
#include "CommandSlider.hpp"
#include "CommandSliderPointer.hpp"
#include "CommandTogglePointer.hpp"
#include "PlayerExcludes.hpp"

namespace Stand
{
	struct TargetingData
	{
		PlayerExcludes excludes;
		bool players = true;
		bool peds = true;
		bool vehicles = false;
		bool objects = false;
		bool use_range = false;
		uint16_t range = 1000;

		void populateOptions(CommandList* list, const std::vector<CommandName>& command_name_prefixes, bool include_objects = false, bool include_range = false, uint16_t max_range = 30000)
		{
			list->createChild<CommandTogglePointer>(&players, LOC("PLYLST"), CommandIssuable::combineCommandNames(command_name_prefixes, CMDNAME("players")));
			list->createChild<CommandTogglePointer>(&peds, LOC("PEDS"), CommandIssuable::combineCommandNames(command_name_prefixes, CMDNAMES("peds", "pedestrians")));
			list->createChild<CommandTogglePointer>(&vehicles, LOC("VEHS"), CommandIssuable::combineCommandNames(command_name_prefixes, CMDNAME("vehicles")));

			if (include_objects)
			{
				list->createChild<CommandTogglePointer>(&objects, LOC("OBJS"), CommandIssuable::combineCommandNames(command_name_prefixes, CMDNAME("objects")));
			}

			if (include_range)
			{
				use_range = true;
				list->createChild<CommandSliderPointer<CommandSlider, uint16_t>>(LOC("DIST"), CommandIssuable::combineCommandNames(command_name_prefixes, CMDNAMES("range")), NOLABEL, &range, 0, max_range);
			}

			list->createChild<CommandDivider>(LOC("XCLDS"));
			excludes.populateList(list, command_name_prefixes, PlayerExcludes::SHOW_ORG_MEMBERS);
		}
	};
}
