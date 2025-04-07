#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandListReadonly.hpp"

namespace Stand
{
	class CommandPlayerInfo : public CommandOnPlayer<CommandListReadonly>
	{
	public:
		explicit CommandPlayerInfo(CommandList* const parent);
	};
}
