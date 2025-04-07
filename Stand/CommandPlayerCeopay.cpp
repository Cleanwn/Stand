#include "CommandPlayerCeopay.hpp"

#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "script_events.hpp"
#include "ScriptGlobal.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerCeopay::CommandPlayerCeopay(CommandList* parent)
		: CommandPlayerToggle(parent, LOC("CEOPAY"), { CMDNAME("ceopay"), CMDNAME("ceocash"), CMDNAME("ceomoney") }, LOC("CEOPAY_H2"), COMMANDPERM_NEUTRAL)
	{
	}

	void CommandPlayerCeopay::onClick(Click& click)
	{
		DEF_P2;
		if (pp->single && pp->getPlayers(true).at(0) == g_player)
		{
			return click.notAvailableOnUser();
		}
		return CommandToggle::onClick(click);
	}

	static time_t timer_1 = 0;
	static bool alt = false;
	static time_t timer_2 = 0;

	void CommandPlayerCeopay::onEnable(Click& click)
	{
		FiberPool::queueJob([this]
		{
			PP_PTR->toggleFiberBatch(getPathEnFromParent(COMMAND_LIST_PLAYER), [](const std::vector<AbstractPlayer>& players)
			{
				if (GET_MILLIS_SINCE(timer_1) > 30000)
				{
					//util::toast("timer_1", TC_SCRIPT);
					for (const AbstractPlayer p : players)
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_CEOPAY),
							g_player,
							10000,
							Util::i32_to_u64_trickery(-1292453789),
							alt,
							(uint64_t)*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_SEVERIFY).as<int*>(),
							(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(9).as<int*>(),
							(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(10).as<int*>(),
						};
						TSE(1 << p, args);
					}
					timer_1 = get_current_time_millis();
					alt = !alt;
				}
				if (GET_MILLIS_SINCE(timer_2) > 120000)
				{
					//util::toast("timer_2", TC_SCRIPT);
					for (const AbstractPlayer p : players)
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_CEOPAY),
							g_player,
							30000,
							Util::i32_to_u64_trickery(198210293),
							1,
							(uint64_t)*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(p, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_SEVERIFY).as<int*>(),
							(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(9).as<int*>(),
							(uint64_t)*ScriptGlobal(GLOBAL_SEVERIFY).at(10).as<int*>(),
						};
						TSE(1 << p, args);
					}
					timer_2 = get_current_time_millis();
				}
			});
		});
	}
}
