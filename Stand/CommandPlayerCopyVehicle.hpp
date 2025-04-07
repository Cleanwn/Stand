#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandPlayerAction.hpp"

namespace Stand
{
	class CommandPlayerCopyVehicle : public CommandOnPlayer<CommandPlayerAction>
	{
	public:
		explicit CommandPlayerCopyVehicle(CommandList* parent);
		
		void onClick(Click& click) final;
	};
}
