#pragma once

#include "CommandColour.hpp"

namespace Stand
{
	class CommandColourPointer : public CommandColour
	{
	private:
		Colour* const colour;

	public:
		explicit CommandColourPointer(CommandList* const parent, Colour* const colour, Label&& menu_name, std::vector<CommandName>&& command_names, CommandColour* with_copy_from = nullptr);

		void onChange(Click& click) final;
	};
}
