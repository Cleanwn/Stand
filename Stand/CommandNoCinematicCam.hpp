#pragma once

#include "CommandToggle.hpp"

#include "Camgr.hpp"

namespace Stand
{
	class CommandNoCinematicCam : public CommandToggle
	{
	public:
		explicit CommandNoCinematicCam(CommandList* const parent)
			: CommandToggle(parent, LOC("NCINCAM"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAMES("cinematiccamera", "cinematicamera")))
		{
		}

		void onEnable(Click& click) final
		{
			Camgr::disableCinematicCam();
		}

		void onDisable(Click& click) final
		{
			Camgr::enableCinematicCam();
		}
	};
}
