#pragma once

#include "CommandActionScript.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "PlayerProvider.hpp"
#include "TpUtil.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	class CommandPlayerTeleport : public CommandActionScript
	{
	public:
		explicit CommandPlayerTeleport(CommandList* const parent)
			: CommandActionScript(parent, LOC("PLYTP"), CMDNAMES("tp", "tpto", "teleport", "teleportto"), NOLABEL, CMDFLAGS_ACTION_SCRIPT | CMDFLAG_TEMPORARY, COMMANDPERM_NEUTRAL)
		{
			PC_PTR->registerCommandNames(this);
		}

		Label getActivationName() const final
		{
			return LIT(LANG_FMT("PLYTP_A", PP_PTR->getPlayer().getName()));
		}

		void onClickScriptThread(Click& click) final
		{
			const AbstractPlayer ap = PP_PTR->getPlayers(true).at(0);
			const AbstractPlayer issuer = click.getEffectiveIssuer();
			if (ap == issuer)
			{
				click.uwotm8();
			}
			else
			{
				if (issuer == g_player)
				{
					TpUtil::teleport(ap.getPos());
				}
				else
				{
					issuer.fiberTeleport(ap.getPos());
				}
			}
		}
	};
}
