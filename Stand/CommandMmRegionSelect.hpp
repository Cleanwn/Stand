#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandMmRegionSelect : public CommandListSelect
	{
	public:
		explicit CommandMmRegionSelect(CommandList* const parent)
			: CommandListSelect(parent, LOC("MMREGION"), {}, NOLABEL, {
				{8, LOC("NFLTR")},
				{5, LOC("Australia")},
				{3, LOC("Europe")},
				{4, LOC("Asia")},
				{7, LOC("Japan")},
				{1, LOC("LATAM")},
				{0, LOC("REGION_NA")},
				{2, LOC("USA_East")},
				{6, LOC("USA_West")},
			}, 8)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (parent->as<CommandMmRegion>()->toggle->m_on)
			{
				pointers::rlRos_s_GeoLocInfo->region_code = (uint32_t)value;
			}
		}

		void setState(Click& click, const std::string& state) final
		{
			// Migrate from 108 and earlier
			if (state == "Hong Kong")
			{
				return CommandListSelect::setState(click, "Asia");
			}

			return CommandListSelect::setState(click, state);
		}
	};
}
