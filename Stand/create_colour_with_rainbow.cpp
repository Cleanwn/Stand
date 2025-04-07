#include "create_colour_with_rainbow.hpp"

#include "CommandColourPointer.hpp"
#include "CommandColourPointerDirectx.hpp"
#include "create_rainbow.hpp"

namespace Stand
{
	void create_colour_with_rainbow(CommandList* list, DirectX::SimpleMath::Color* colour, std::vector<CommandName>&& command_names)
	{
		create_rainbow(list->createChild<CommandColourPointerDirectx>(colour, LOC("CLR"), std::move(command_names)));
	}

	void create_colour_with_rainbow(CommandList* list, Colour* colour, std::vector<CommandName>&& command_names)
	{
		create_rainbow(list->createChild<CommandColourPointer>(colour, LOC("CLR"), std::move(command_names)));
	}
}
