#include "CommandDisableInVehicleHairScale.hpp"

#include <soup/macros.hpp>

#include "pointers.hpp"

namespace Stand
{
	CommandDisableInVehicleHairScale::CommandDisableInVehicleHairScale(CommandList* const parent)
		: CommandToggle(parent, LOC("NVEHHAIRSCL"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAME("hairscaling")))
	{
	}

	void CommandDisableInVehicleHairScale::onEnable(Click& click)
	{
		if (pointers::ShouldPerformInVehicleHairScale_jz)
		{
			uint8_t patch[] = {
				/* 0x0F, 0x84 -> */ 0x90, 0xE9 // jz -> jmp
			};
			m_patch.initPatch(pointers::ShouldPerformInVehicleHairScale_jz, patch, COUNT(patch));
		}
	}

	void CommandDisableInVehicleHairScale::onDisable(Click& click)
	{
		m_patch.restore();
	}
}
