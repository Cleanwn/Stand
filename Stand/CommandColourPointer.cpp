#include "CommandColourPointer.hpp"

#include "Colour.hpp"

namespace Stand
{
	CommandColourPointer::CommandColourPointer(CommandList* const parent, Colour* const colour, Label&& menu_name, std::vector<CommandName>&& command_names, CommandColour* with_copy_from)
		: CommandColour(parent, std::move(menu_name), std::move(command_names), NOLABEL, colour->r, colour->g, colour->b, -1, CMDFLAGS_COLOUR, with_copy_from), colour(colour)
	{
	}

	void CommandColourPointer::onChange(Click& click)
	{
		getRGB(*colour);
	}
}
