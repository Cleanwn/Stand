#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandAction.hpp"

namespace Stand
{
	class CommandPlayerPong : public CommandOnPlayer<CommandAction>
	{
	public:
		explicit CommandPlayerPong(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
