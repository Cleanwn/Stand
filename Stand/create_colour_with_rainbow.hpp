#pragma once

#include "dx_common.hpp"
#include "fwddecl.hpp"

namespace Stand
{
	extern void create_colour_with_rainbow(CommandList* list, DirectX::SimpleMath::Color* colour, std::vector<CommandName>&& command_names);
	extern void create_colour_with_rainbow(CommandList* list, Colour* colour, std::vector<CommandName>&& command_names);
}
