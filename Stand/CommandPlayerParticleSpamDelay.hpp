#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandSlider.hpp"

namespace Stand
{
	class CommandPlayerParticleSpamDelay : public CommandOnPlayer<CommandSlider>
	{
	public:
		explicit CommandPlayerParticleSpamDelay(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("INTVAL_MS"), CMDNAMES_OBF("ptfxdelay"), NOLABEL, 0, 1000, 0, 50)
		{
			PC_PTR->registerCommand(this);
		}
	};
}