#pragma once

#include "CommandList.hpp"

#include "AbstractEntity.hpp"
#include "CommandAutoDriveAction.hpp"
#include "CommandLambdaActionScript.hpp"
#include "CommandListSelectDrivingMode.hpp"
#include "CommandSliderFloatOverride.hpp"

namespace Stand
{
	class CommandAutodrive : public CommandList
	{
	public:
		explicit CommandAutodrive(CommandList* const parent)
			: CommandList(parent, LOC("AUTODRIVE"), CMDNAMES_OBF("autodrive"))
		{
			auto action = createChild<CommandAutoDriveAction>();

			createChild<CommandLambdaActionScript>(LOC("CANCL"), CMDNAMES_OBF("stopautodrive"), NOLABEL, [action](const Click& click)
			{
				TASK::CLEAR_PED_TASKS(g_player_ped);
				action->requestDeath();
			});

			action->speed = createChild<CommandSliderFloatOverride>(LOC("AUTODRIVE_C"), CMDNAMES_OBF("autodrivespeed"), NOLABEL, 0, 8000, 0, 100, 0);
			action->mode = createChild<CommandListSelectDrivingMode>();
			action->beacon = createChild<CommandToggle>(LOC("AUTODRIVE_SDB"), CMDNAMES_OBF("showbeacon"));
			action->auto_kill = createChild<CommandToggle>(LOC("AUTODRIVE_ACOI"), CMDNAMES_OBF("autodriveautocancel"), LOC("AUTODRIVE_ACOI_H"));
			action->noclipping = createChild<CommandToggle>(LOC("AUTODRIVE_NOCLIP"), CMDNAMES_OBF("noclipwhileautodrive"), LOC("AUTODRIVE_NOCLIP_H"));
		}
	};
}