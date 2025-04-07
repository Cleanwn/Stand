#pragma once

#include "CommandList.hpp"

#include "CommandSliderRegenerationRate.hpp"

namespace Stand
{
	class CommandRegenerationRate : public CommandList
	{
	public:
		explicit CommandRegenerationRate(CommandList* const parent)
			: CommandList(parent, LOC("REGEN"), CMDNAMES_OBF("regenrate"))
		{
			auto health = createChild<CommandSliderRegenerationRate>(LOC("HLTH"), CMDNAMES_OBF("healthrate"), LOC("REGEN_H_H"), CommandSliderRegenerationRate::HEALTH);
			auto armour = createChild<CommandSliderRegenerationRate>(LOC("ARMR"), CMDNAMES_OBF("armourrate"), LOC("REGEN_A_H"), CommandSliderRegenerationRate::ARMOUR);
			createChild<CommandDivider>(LOC("STTNGS"));
			health->only_in_cover = armour->only_in_cover = createChild<CommandToggle>(LOC("ONLYINCOVER"), CMDNAMES_OBF("regenonlyincover"));
		}
	};
}