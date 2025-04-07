#pragma once

#include "CommandPlayerToggle.hpp"

#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerTransactionError : public CommandPlayerToggle
	{
	public:
		explicit CommandPlayerTransactionError(CommandList* const parent)
			: CommandPlayerToggle(parent, LOC("PLYERR"), { CMDNAME("error") }, NOLABEL, COMMANDPERM_AGGRESSIVE)
		{
		}

		void onEnable(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				PP_PTR->toggleBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
				{
					for (AbstractPlayer p : players)
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_TRANSACTION_ERROR),
							g_player,
							0x7FFFFFFF, // 2: Amount
							0, // 3: Don't Notify
							0,
							(uint64_t)*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_SEVERIFY).as<int*>(),
							(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(9).as<int*>(),
							(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(10).as<int*>(),
						};
						TSE(1 << p, args);
					}
				});
			});
		}
	};
}
