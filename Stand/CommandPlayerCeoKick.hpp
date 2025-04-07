#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "AbstractPlayer.hpp"
#include "ExecCtx.hpp"
#include "OrgHelper.hpp"
#include "script_events.hpp"
#include "tse.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandPlayerCeoKick : public CommandPlayerActionAggressive
	{
	private:
		inline static RateLimit scripted_click_ratelimit{ 15000, 1 }; // once every 15 seconds

	public:
		explicit CommandPlayerCeoKick(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("CEOKICK"), CMDNAMES_OBF("ceokick", "mckick"))
		{
		}

		void onClick(Click& click) final
		{
			if (click.type == CLICK_SCRIPTED)
			{
				if (!scripted_click_ratelimit.canRequest())
				{
					return;
				}
				scripted_click_ratelimit.addRequest();
			}

			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this, click](ThreadContext)
			{
				AbstractPlayer p = getPlayer();
				if (!p.isInAnOrg())
				{
					click.setResponse(LIT(LANG_FMT("NOCEO", p.getName())));
					return;
				}
				if (p == g_player)
				{
					ExecCtx::get().ensureScript([]
					{
						OrgHelper::leave();
					});
					return;
				}
				if (p.isBoss())
				{
					ExecCtx::get().ensureScript([p]
					{
						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_CEO_KICK),
							g_player,
							p.getBoss()
						};
						TSE(1 << p, args);
					});
					//click.setResponse(LOC("CEOKICK_E"));
					return;
				}
				ExecCtx::get().ensureScript([p]
				{
					uint64_t args[] = {
						Util::i32_to_u64_trickery(SE_MARK_AS_BEAST),
						g_player,
						p.getBoss()
					};
					TSE(1 << p, args);
				});
			});
		}
	};
}
