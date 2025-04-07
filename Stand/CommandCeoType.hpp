#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandCeoType : public CommandAction
	{
	public:
		explicit CommandCeoType(CommandList* parent)
			: CommandAction(parent, LOC("CEOTYP"), { CMDNAME("ceotomc"), CMDNAME("mctoceo") }, LOC("CEOTYP_H"))
		{
		}

		void onClick(Click& click) final
		{
			if (click.isBoss())
			{
				int type = (g_player.isInMc() ? 0 : 1);
				ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_TYPE).set<int>(type);
				ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_TYPE_COPY).set<int>(type);

				auto name = StatUtil::getDualString(g_player.isInMc() ? "mc_gang_name" : "gb_gang_name");
				g_player.setOrgName(name.c_str());
			}
		}
	};
}
