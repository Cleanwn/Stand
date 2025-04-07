#pragma once

#include "CommandToggle.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandNoIdleCam : public CommandToggle
	{
	public:
		explicit CommandNoIdleCam(CommandList* const parent)
			: CommandToggle(parent, LOC("NIDLECAM"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("idlecam")))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
				CAM::INVALIDATE_CINEMATIC_VEHICLE_IDLE_MODE();
				CAM::INVALIDATE_IDLE_CAM();
			});
		}
	};
}
