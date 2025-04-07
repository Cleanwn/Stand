#include "CommandPlayerCopyOutfit.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "lang.hpp"
#include "PlayerProvider.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandPlayerCopyOutfit::CommandPlayerCopyOutfit(CommandList* const parent)
		: CommandOnPlayer(parent, LOC("CPOTFT"), CMDNAMES_OBF("copyoutfit"), NOLABEL, CMDFLAGS_ACTION_SCRIPT | CMDFLAG_TEMPORARY)
	{
		PC_PTR->registerCommand(this);
	}

	void CommandPlayerCopyOutfit::onClickScriptThread(Click& click)
	{
		const AbstractPlayer p = PP_PTR->getPlayer();
		if (p == g_player)
		{
			click.uwotm8();
		}
		else
		{
			auto ped = p.getPed();
			Util::loadOutfit(this, click, Util::get_colons_outfit_from_ped(ped));
		}
	}
}
