#include "CommandSessionThunder.hpp"

#include "AbstractPlayer.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandTogglePointer.hpp"
#include "GlobalsWatcher.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	CommandSessionThunder::CommandSessionThunder(CommandList* const parent)
		: CommandList(parent, LOC("THDR"))
	{
		createChild<CommandLambdaActionScript>(LOC("THDR_ON"), CMDNAMES("thunderon"), NOLABEL, [](const Click& click)
		{
			if (click.inOnline())
			{
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MHBITSET).at(0, 1).as<int*>() |= (1 << eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather);
			}
		});
		createChild<CommandLambdaActionScript>(LOC("THDR_OFF"), CMDNAMES("thunderoff"), NOLABEL, [](const Click& click)
		{
			if (click.inOnline())
			{
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MHBITSET).at(0, 1).as<int*>() &= ~(1 << eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather);
			}
		});
		createChild<CommandTogglePointer>(&GlobalsWatcher::notify_thunder_request, LOC("THDR_N"), CMDNAMES("thundernotify"));
	}
}
