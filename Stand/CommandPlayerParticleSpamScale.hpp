#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandPlayerParticleSpamScale : public CommandOnPlayer<CommandSliderFloat>
	{
	public:
		explicit CommandPlayerParticleSpamScale(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("SCLE"), CMDNAMES_OBF("ptfxscale"), NOLABEL, 25, 1000, 100, 50)
		{
			PC_PTR->registerCommand(this);
		}
	};
}