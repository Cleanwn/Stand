#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "ScriptGlobal.hpp"
#include "tse.hpp"

namespace Stand
{
	class CommandPlayerInteriorKick : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerInteriorKick(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("INTKICK"), CMDNAMES_OBF("interiorkick"))
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, [this]
				{
					for (auto& p : PP_PTR->getPlayers())
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_KICK_FROM_INTERIOR),
							g_player,
							(uint64_t)ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_INT_ID).get<int>(),
							(uint64_t)ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(p, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_INT_INST).get<int>(),
							0,
							0,
							0,
							0
						};

						TSE(1 << p, args);
					}
				});
			});
		}
	};
}