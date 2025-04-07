#include "CommandPlayerBefriend.hpp"

#include "AbstractPlayer.hpp"
#include "CommandListPlayer.hpp"
#include "Gui.hpp"
#include "PlayerProvider.hpp"
#include "players_interface.hpp"
#include "pointers.hpp"
#include "rlPc.hpp"

namespace Stand
{
	CommandPlayerBefriend::CommandPlayerBefriend(CommandList* const parent)
		: CommandActionScript(parent, LOC("PLYFRND"), CMDNAMES_OBF("befriend"), NOLABEL, CMDFLAGS_ACTION_SCRIPT | CMDFLAG_TEMPORARY)
	{
		PC_PTR->registerCommandNames(this);
	}

	void CommandPlayerBefriend::onClickScriptThread(Click& click)
	{
		g_gui.prepareForOverlay();
		pointers::g_rlPc->player_manager->ShowFriendRequestUi(PP_PTR->getPlayers(true)[0].getRockstarId());
	}
}
