#include "CommandPlayerKickPoolsClosed.hpp"

#include "Hooking.hpp"
#include "Script.hpp"
#include "ScriptHostUtil.hpp"
#include "ScriptGlobal.hpp"
#include "script_handler.hpp"
#include "script_thread.hpp"

namespace Stand
{
	CommandPlayerKickPoolsClosed::CommandPlayerKickPoolsClosed(CommandList* const parent)
		: CommandPlayerActionAggressive(parent, LOC("PLYKCK_PC"), CMDNAMES_OBF("aids"), LOC("PLYKCK_H_B_P"))
	{
	}

	void CommandPlayerKickPoolsClosed::onClick(Click& click)
	{
		if (PP_PTR->isUser())
		{
			click.notAvailableOnUser();
			return;
		}
		ensureYieldableScriptThread(click, [this]
		{
			auto t = GtaThread::fromHash(ATSTRINGHASH("freemode"));
			if (!t)
			{
				return;
			}
			auto netcomp = t->getNetComponent();
			if (netcomp
				&& ScriptHostUtil::canSafelyTakeIt() // Avoid people using this as a thunderjoin alternative.
				&& !g_hooking.pools_closed_kick.isActive()
				)
			{
				const auto targets = PP_PTR->getPlayers();

				// When we are SH, we can only use "Pool's Closed" once.
				// We work around this by giving up SH for a few sec if we just were the SH.
				if (netcomp->amHost())
				{
					netcomp->giveHost(ScriptHostUtil::getViableSh().getCNetGamePlayer());
					Script::current()->yield(3000);
				}

				if (t->fiberSnatchScriptHost()
					&& setWhitelistTargeting(g_hooking.pools_closed_kick, targets)
					)
				{
					for (const auto& p : targets)
					{
						p.onPreRemove();
						ScriptHostUtil::pool_closed_at[p] = get_current_time_millis();
					}
					Script::current()->yield(3000);
					g_hooking.pools_closed_kick.reset();
					ScriptGlobal(GSBD).set<int>(4);
				}
			}
		});
	}
}
