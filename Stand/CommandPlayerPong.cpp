#include "CommandPlayerPong.hpp"

#include "Auth.hpp"
#include "CommandPlayer.hpp"
#include "DirectPacket.hpp"
#include "Pong.hpp"

namespace Stand
{
	CommandPlayerPong::CommandPlayerPong(CommandList* const parent)
		: CommandOnPlayer<CommandAction>(parent, LOC("INVSEND_PONG"), CMDNAMES_OBF("pong"), NOLABEL, CMDFLAGS_ACTION | CMDFLAG_TEMPORARY)
	{
		PC_PTR->registerCommandNames(this);
	}

	void CommandPlayerPong::onClick(Click& click)
	{
		if (!g_auth.isSuiEnabledThisSession())
		{
			click.setResponse(LOC("CMDSUI"));
			return;
		}
		auto* const pc = (CommandPlayer*)PC_PTR;
		if (!pc->isStandUser())
		{
			click.setResponse(LOC("CMDSTAND"));
			return;
		}
		ensureScriptThread(click, [this]
		{
			auto* const pc = (CommandPlayer*)PC_PTR;
			if (Pong::participant != nullptr)
			{
				const bool is_cancel = (Pong::participant == pc);
				if (Pong::state >= Pong::PLAYING)
				{
					Pong::participant->getPlayer().directPacketSend(DP_PONG_BYE);
				}
				Pong::participant->pong_command->setMenuName(LOC("INVSEND_PONG"));
				Pong::stopGame();
				if (is_cancel)
				{
					return;
				}
			}
			Pong::participant = (CommandPlayer*)pc;
			Pong::participant->getPlayer().directPacketSend(DP_PONG_INVITE, Pong::participant->got_pong_invite ? 1 : 0);
			Pong::i_am_left = !Pong::participant->got_pong_invite;
			Pong::participant->got_pong_invite = false;
			Pong::state = Pong::INVITE_SENT;
			setMenuName(LOC("CNLINV"));
		});
	}
}
