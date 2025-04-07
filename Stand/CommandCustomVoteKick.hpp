#pragma once

#include "CommandList.hpp"

#include "CommandSlider.hpp"
#include "CommandCustomVoteKickToggle.hpp"

namespace Stand
{
	class CommandCustomVoteKick : public CommandList
	{
	public:
		explicit CommandCustomVoteKick(CommandList* const parent)
			: CommandList(parent, LOC("CSTMVK"))
		{
			auto toggle = createChild<CommandCustomVoteKickToggle>();
			toggle->threshold_slider = createChild<CommandSlider>(LOC("THRESHOLD"), CMDNAMES_OBF("customvotekickthreshold"), LOC("CSTMVK_H"), 1, 32, 5);
			createChild<CommandDivider>(LOC("XCLDS"));
			toggle->exclusions.populateList(this, CMDNAMES_OBF("customvotekick"), PlayerExcludes::SHOW_ORG_MEMBERS);
			createChild<CommandDivider>(LOC("NOTFS"));
			toggle->toast_config.populateList(this, CMDNAMES_OBF("customvotekick"));
		}
	};
}
