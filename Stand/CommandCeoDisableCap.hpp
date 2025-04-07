#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandCeoDisableCap : public CommandToggle
	{
	public:
		explicit CommandCeoDisableCap(CommandList* parent)
			: CommandToggle(parent, LOC("CEOCAP"), CMDNAMES("ceonocap", "mcnocap"), LOC("GEN_H_BOSS"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				if (g_player.isBoss())
				{
					ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MEMBERS).at(0, 1).set<int>(-1);
				}
			});
		}
	};
}
