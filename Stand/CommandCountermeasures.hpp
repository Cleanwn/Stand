#pragma once

#include "CommandList.hpp"

#include "CommandAutoDeployCountermeasures.hpp"
#include "CommandFireCountermeasure.hpp"

namespace Stand
{
	class CommandCountermeasures : public CommandList
	{
	private:
		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions()
		{
			using FlarePattern = CommandFireCountermeasure::FlarePattern;

			return std::vector<CommandListActionItemData>{
				CommandListActionItemData(FlarePattern::HORIZONTAL, LOC("CM_HOR")),
				CommandListActionItemData(FlarePattern::HORIZONTAL_DOWNARDS, LOC("CM_HOR_DOWN")),
				CommandListActionItemData(FlarePattern::HORIZONTAL_UPWARDS, LOC("CM_HOR_UP")),
				CommandListActionItemData(FlarePattern::VERTICAL, LOC("CM_VERT")),
				CommandListActionItemData(FlarePattern::VERTICAL_DOWNWARDS, LOC("CM_VERT_DOWN")),
				CommandListActionItemData(FlarePattern::RANDOM, LOC("RNG")),
			};
		}

	public:
		explicit CommandCountermeasures(CommandList* const parent)
			: CommandList(parent, LOC("CM"), CMDNAMES_OBF("countermeasures"))
		{
			auto count = makeChild<CommandSlider>(LOC("COUNT"), CMDNAMES_OBF("cmcount"), NOLABEL, 1, 10, 2);
			auto onlyair = makeChild<CommandToggle>(LOC("CM_OF"), CMDNAMES_OBF("onlyaircraft"));
			auto pattern = makeChild<CommandListSelect>(LOC("CM_PT"), CMDNAMES_OBF("flarepattern"), NOLABEL, getOptions(), 0);
			auto periodic = makeChild<CommandToggle>(LOC("CM_PERIOD"), CMDNAMES_OBF("periodiccm", "autocm"));
			auto periodic_delay = makeChild<CommandSlider>(LOC("CM_PERIOD_D"), CMDNAMES_OBF("periodiccmdelay", "autocmdelay"), NOLABEL, 1, 10, 2);
			createChild<CommandAutoDeployCountermeasures>(periodic.get(), periodic_delay.get());
			createChild<CommandFireCountermeasure>(LOC("CM_CHAFF"), CMDNAMES_OBF("deploychaff"), count.get(), onlyair.get(), pattern.get(), CommandFireCountermeasure::CHAFF);
			createChild<CommandFireCountermeasure>(LOC("CM_FLARE"), CMDNAMES_OBF("deployflares"), count.get(), onlyair.get(), pattern.get(), CommandFireCountermeasure::FLARE);
			createChild<CommandFireCountermeasure>(LOC("CM_BOTH"), CMDNAMES_OBF("deployboth"), count.get(), onlyair.get(), pattern.get(), CommandFireCountermeasure::BOTH);
			createChild<CommandDivider>(LOC("STTNGS"));
			children.emplace_back(std::move(count));
			children.emplace_back(std::move(onlyair));
			children.emplace_back(std::move(pattern));
			children.emplace_back(std::move(periodic));
			children.emplace_back(std::move(periodic_delay));
		}
	};
}