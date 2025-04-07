#include "apply_spoofed_name.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "CPlayerInfo.hpp"
#include "Hooking.hpp"
#include "pointers.hpp"
#include "rlGamerInfo.hpp"

namespace Stand
{
	void apply_spoofed_name(const char* name)
	{
		g_player.setOverrideName(name);
		strcpy(pointers::rlPresence_m_ActingGamerInfo->name, name);
	}

	void on_name_spoofing_update()
	{
		if (auto cmd = g_player.getCommand())
		{
			cmd->force_recreate = true;
		}
	}
}
