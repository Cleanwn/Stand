#include "CommandPlayerCeoJoin.hpp"

#include <fmt/core.h>

#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "lang.hpp"
#include "OrgHelper.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerCeoJoin::CommandPlayerCeoJoin(CommandList* const parent)
		: CommandPlayerToggle(parent, LOC("CEOJOIN"), CMDNAMES_OBF("ceojoin", "mcjoin"))
	{
	}

	void CommandPlayerCeoJoin::onCommand(Click& click, std::wstring& args)
	{
		onTickInGameViewport();
		CommandPlayerToggle::onCommand(click, args);
	}

	static compactplayer_t spoofed_boss = MAX_PLAYERS;

	void CommandPlayerCeoJoin::onEnable(Click& click)
	{
		if (!click.isStand())
		{
			click.stopInputIfAllowed();
			const auto boss = AbstractPlayer(getPlayer()).getBoss();
			if (!boss.isValid())
			{
				click.setResponse(LIT(LANG_FMT("NOCEO", getPlayerName())));
				return;
			}
			FiberPool::queueJob([boss]
			{
				// Ensure this is the only ceojoin fiber
				if (spoofed_boss != MAX_PLAYERS)
				{
					spoofed_boss = MAX_PLAYERS;
					Script::current()->yield();
				}
				spoofed_boss = boss;

				// Attempt graceful join
				Util::toast(LOC("CEOJOIN_T"));
				OrgHelper::join(boss);
				time_t deadline = get_current_time_millis() + 3000;
				do
				{
					Script::current()->yield();
				} while (!OrgHelper::hasBossRespondedToJoinRequest()
					&& boss == spoofed_boss
					&& get_current_time_millis() < deadline
					);
				if (OrgHelper::hasBossAcceptedJoinRequest())
				{
					return;
				}

				// Graceful join did not work, will need to fake that we received a response and maintain this fiber.
				if (!AbstractPlayer(boss).exists()
					|| !AbstractPlayer(boss).isBoss()
					)
				{
					return;
				}
				Util::toast(LOC("CEOJOIN_E"));
				*ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_FLAGS).as<int*>() |= (1 << GLOBAL_ORG_FLAGS_INVITEACCEPTED);

				do
				{
					ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(boss, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MEMBERS).at(0, 1).set<int>(g_player);
					Script::current()->yield();
				} while (boss == spoofed_boss
					&& AbstractPlayer(boss).exists()
					&& AbstractPlayer(boss).isBoss()
					);

				if (AbstractPlayer(boss).exists()
					&& AbstractPlayer(boss).isBoss())
				{
					ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(boss, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_MEMBERS).at(0, 1).set<int>(-1);
				}
			});
		}
	}

	void CommandPlayerCeoJoin::onDisable(Click& click)
	{
		if (!click.isStand())
		{
			click.stopInputIfAllowed();
			ensureYieldableScriptThread(click, []
			{
				spoofed_boss = MAX_PLAYERS;
				Script::current()->yield();
				OrgHelper::leave();
			});
		}
	}

	void CommandPlayerCeoJoin::onTickInGameViewport()
	{
		Click click{ CLICK_AUTO, TC_SCRIPT_NOYIELD };
		const auto boss = AbstractPlayer(getPlayer()).getBoss();
		setStateBool(click, (boss.isValid() && boss == g_player.getBoss()));
	}

	void CommandPlayerCeoJoin::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
