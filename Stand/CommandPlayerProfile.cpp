#include "CommandPlayerProfile.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "Gui.hpp"
#include "PlayerProvider.hpp"
#include "players_interface.hpp"
#include "pointers.hpp"
#include "rlPc.hpp"

namespace Stand
{
	CommandPlayerProfile::CommandPlayerProfile(CommandList* const parent)
		: CommandActionScript(parent, LOC("PLYPRFL"), CMDNAMES_OBF("profile"), NOLABEL, CMDFLAGS_ACTION_SCRIPT | CMDFLAG_TEMPORARY)
	{
		PC_PTR->registerCommandNames(this);
	}

	void CommandPlayerProfile::onClickScriptThread(Click& click)
	{
		g_gui.prepareForOverlay();
		pointers::g_rlPc->player_manager->ShowPlayerProfileUi(PP_PTR->getPlayers(true)[0].getRockstarId());
	}
}
