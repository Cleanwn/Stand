#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandActionScript.hpp"

#include <soup/rand.hpp>

namespace Stand
{
	class CommandPlayerGift : public CommandOnPlayer<CommandActionScript>
	{
	public:
		explicit CommandPlayerGift(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("PLYGIFT"), { CMDNAME("gift") }, LOC("PLYGIFT_H"), CMDFLAGS_ACTION_SCRIPT | CMDFLAG_TEMPORARY, COMMANDPERM_FRIENDLY)
		{
			PC_PTR->registerCommandNames(this);
		}

	private:
		static void giftImpl(const Click& click, const AbstractPlayer p)
		{
			p.openThirdEye([click](const AbstractPlayer& p, AbstractEntity& ped, bool needs_cleanup)
			{
				auto veh = ped.getVehicle(false);
				if (!veh.isValid())
				{
					click.setResponse(LIT(LANG_FMT("PLYNVEH", p.getName())));
					p.openThirdEyeCleanup(needs_cleanup);
					return;
				}
				veh.requestControl(ATSTRINGHASH("gift"), [p, click, needs_cleanup](AbstractEntity& veh)
				{
					DECORATOR::DECOR_SET_INT(veh, "MPBitset", 0);
					const Hash player_network_hash = NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(p);
					DECORATOR::DECOR_SET_INT(veh, "Player_Vehicle", player_network_hash);
					DECORATOR::DECOR_SET_INT(veh, "Previous_Owner", player_network_hash);
					DECORATOR::DECOR_SET_INT(veh, "Veh_Modded_By_Player", player_network_hash);
					DECORATOR::DECOR_SET_INT(veh, "PV_Slot", soup::rand.t<int>(0, 100));
					click.setResponse(LIT(LANG_FMT("PLYGIFT_T", p.getName())));
					p.openThirdEyeCleanup(needs_cleanup);
				}, [p, click, needs_cleanup](AbstractEntity& veh)
				{
					click.setResponse(LIT(LANG_FMT("PLYVEHNCTRL", p.getName())));
					p.openThirdEyeCleanup(needs_cleanup);
				});
			});
		}

	public:
		void onClickScriptThread(Click& click) final
		{
			if (!click.inOnline())
			{
				return;
			}
			click.setNoResponse();
			const AbstractPlayer ap = PP_PTR->getPlayers(true).at(0);
			if (click.issuedByUser())
			{
				showWarning(click, fmt::format(fmt::runtime(LANG_GET_W("PLYGIFT_W")), ap.getNameW()), 0, [this, click, ap](ThreadContext thread_context)
				{
					ensureYieldableScriptThread(thread_context, [click, ap]
					{
						giftImpl(click, ap);
					});
				});
			}
			else
			{
				ensureYieldableScriptThread(click, [click, ap]
				{
					giftImpl(click, ap);
				});
			}
		}
	};
}
