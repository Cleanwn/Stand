#pragma once

#include "CommandTextslider.hpp"

#include "JoinMethod.hpp"

namespace Stand
{
	class CommandTextsliderJoin : public CommandTextslider
	{
	public:
		explicit CommandTextsliderJoin(CommandList* parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, commandflags_t extra_flags = 0)
			: CommandTextslider(parent, std::move(menu_name), std::move(command_names), NOLABEL, std::vector<CommandTextsliderData>{
				//{ JM_NUTS, LOC("JM_NUTS") },
				{ JM_NINJA, LOC("JM_N") },
				//{ JM_MATCHMAKING, LOC("JM_M") },
				{ JM_FRIEND, LOC("JM_F") },
			}, CMDFLAGS_ACTION_SLIDER | CMDFLAG_FEATURELIST_BASIC | extra_flags)
		{
		}
	};
}
