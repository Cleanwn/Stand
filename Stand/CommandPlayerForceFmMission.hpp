#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "script_events.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerForceFmMission : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerForceFmMission(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("PLYMISSION"), { CMDNAME("mission") }, LOC("PLYMISSION_H"))
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, [this]
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						// 142: Sightseer
						// 263: Nothing?
						// 264: INVESTIGATION: The Marina
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_FM_MISSION),
							g_player,
							Util::i32_to_u64_trickery(263),
							(uint64_t)-1
						};
						TSE(1 << p, args);
					}
				});
			});
		}
	};
}
