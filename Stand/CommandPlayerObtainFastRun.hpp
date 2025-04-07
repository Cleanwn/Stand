#pragma once

#include "RemoteGlobalWriter.hpp"
#if REMOTE_STAT_WRITING

#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerObtainFastRun : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerObtainFastRun(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("ABILOPT"), CMDNAMES_OBF("givefastrun"))
		{
		}

		void onClick(Click& click) final
		{
			ensureYieldableScriptThread(click, [this, click]
			{
				RemoteGlobalWriter w;
				if (w.prepare(PP_PTR->getPlayers()))
				{
					w.setCharStatInt(soup::ObfusString("CHAR_ABILITY_1_UNLCK").str(), -1);
					w.setCharStatInt(soup::ObfusString("CHAR_ABILITY_2_UNLCK").str(), -1);
					w.setCharStatInt(soup::ObfusString("CHAR_ABILITY_3_UNLCK").str(), -1);
					w.setCharStatInt(soup::ObfusString("CHAR_FM_ABILITY_1_UNLCK").str(), -1);
					w.setCharStatInt(soup::ObfusString("CHAR_FM_ABILITY_2_UNLCK").str(), -1);
					w.setCharStatInt(soup::ObfusString("CHAR_FM_ABILITY_3_UNLCK").str(), -1);
				}
				click.setResponse(LOC("GEN_T_NEWSESS_O"));
			});
		}
	};
}
#endif
