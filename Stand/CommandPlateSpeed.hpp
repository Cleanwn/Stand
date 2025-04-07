#pragma once

#include "CommandToggle.hpp"

#include "PlateDj.hpp"

namespace Stand
{
	class CommandPlateSpeed : public CommandToggle
	{
	public:
		explicit CommandPlateSpeed(CommandList* const parent)
			: CommandToggle(parent, LOC("PLATESPEED"), CMDNAMES("platespeedometer"))
		{
		}
		
		Label getActivationName() const final
		{
			return LOC("PLATESPEED_A");
		}

		void onEnable(Click& click) final
		{
			g_plate_dj.enableTrack(PlateDj::SPEEDOMETER);
		}

		void onDisable(Click& click) final
		{
			g_plate_dj.disableTrack(PlateDj::SPEEDOMETER);
		}
	};
}
