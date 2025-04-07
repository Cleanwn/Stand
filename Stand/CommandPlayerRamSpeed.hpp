#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandSlider.hpp"

namespace Stand
{
	class CommandPlayerRamSpeed : public CommandOnPlayer<CommandSlider>
	{
	public:
		explicit CommandPlayerRamSpeed(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("SPD"), CMDNAMES_OBF("ramspeed"), NOLABEL, 20, 200, 40)
		{
			PC_PTR->registerCommandNames(this);
		}
	};
}