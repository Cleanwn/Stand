#pragma once

#include "CommandPlayerCrash.hpp"

namespace Stand
{
	class CommandPlayerCrashTask : public CommandPlayerCrash
	{
	public:
		explicit CommandPlayerCrashTask(CommandList* const parent)
			: CommandPlayerCrash(parent, LIT("[Debug Build] The Buggles"), {}, NOLABEL, CMDFLAGS_ACTION | CMDFLAG_FEATURELIST_SKIP)
		{
		}

		void expandDong(std::vector<AbstractPlayer>&& targets) const final
		{
			if (!g_hooking.outgoing_player_sync_exclusive.isActive())
			{
				auto target = targets.at(0);
				g_hooking.outgoing_player_sync_exclusive.add(target.getCNetGamePlayer());
				g_player_ent.setPos(target.getPos());
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
				STREAMING::REQUEST_ANIM_DICT("anim@mp_player_intupperstinker");
				TASK::TASK_SWEEP_AIM_POSITION(g_player_ped, "anim@mp_player_intupperstinker", "", "", "", -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
				Script::current()->yield(1000);
				TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
				g_hooking.outgoing_player_sync_exclusive.reset();
			}
		}
	};
}
