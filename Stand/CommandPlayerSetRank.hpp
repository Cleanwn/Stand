#pragma once

#include "RemoteGlobalWriter.hpp"
#if REMOTE_STAT_WRITING

#include "CommandPlayerActionValue.hpp"

namespace Stand
{
	class CommandPlayerSetRank : public CommandPlayerActionValue
	{
	public:
		explicit CommandPlayerSetRank(CommandList* const parent)
			: CommandPlayerActionValue(parent, 1, 8000, LOC("SETLVL"), CMDNAMES_OBF("psetrank", "psetlevel"), NOLABEL, COMMANDPERM_NEUTRAL)
		{
		}

		void onCommandWithValue(Click& click, long long value) final
		{
			SOUP_ASSERT(ExecCtx::get().tc == TC_SCRIPT_YIELDABLE);
			int rp = Util::get_rp_required_for_rank((int)value);
			if (rp == 0)
			{
				rp = 1;
			}
			RemoteGlobalWriter w;
			if (w.prepare(PP_PTR->getPlayers()))
			{
				w.setCharStatInt(soup::ObfusString("CHAR_SET_RP_GIFT_ADMIN").str(), rp);
			}
			click.setResponse(LOC("GEN_T_NEWSESS_O"));
		}
	};
}
#endif
