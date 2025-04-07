#include "CommandListSelectDrivingMode.hpp"

#include "AbstractEntity.hpp"
#include "CommandAutoDriveAction.hpp"
#include "eDrivingFlags.hpp"
#include "eScriptTaskStatus.hpp"
#include "Gui.hpp"
#include "Vector2Plus.hpp"

namespace Stand
{
	CommandListSelectDrivingMode::CommandListSelectDrivingMode(CommandList* const parent)
		: CommandListSelect(parent, LOC("AUTODRIVE_DRIVE"), CMDNAMES_OBF("autodrivestyle"), NOLABEL, {
				{ eDrivingFlags::CUSTOM_Lawful, LOC("AUTODRIVE_LAW") },
				{ eDrivingFlags::CUSTOM_Lawful_RunLights, LOC("AUTODRIVE_NOTSOLAWFUL") },
				{ eDrivingFlags::CUSTOM_Aggressive, LOC("SPFDHTPRST_2") },
				{ eDrivingFlags::CUSTOM_Backwards, LOC("AUTODRIVE_BACK") },
			}, eDrivingFlags::CUSTOM_Lawful)
	{
	}

	void CommandListSelectDrivingMode::onChange(Click& click, long long prev_value)
	{
		if (g_gui.isUnloadPending())
		{
			return;
		}

		ensureScriptThread(click, [this]
		{
			if (TASK::GET_SCRIPT_TASK_STATUS(g_player_ped, ATSTRINGHASH("script_task_vehicle_drive_wander")) != FINISHED_TASK
				|| TASK::GET_SCRIPT_TASK_STATUS(g_player_ped, ATSTRINGHASH("script_task_vehicle_drive_to_coord_longrange")) != FINISHED_TASK
				)
			{
				auto cmd = this->parent->resolveChildByMenuName(LOC("AUTODRIVE_DEST"))->as<CommandAutoDriveAction>();
				cmd->applyBehaviour(std::move(cmd->destination_coords));
			}
		});
	}
}