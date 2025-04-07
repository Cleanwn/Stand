#pragma once

#include "CommandColourCustom.hpp"

namespace Stand
{
	class CommandColour : public CommandColourCustom
	{
	public:
		explicit CommandColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int default_r, int default_g, int default_b, int default_a = -1, commandflags_t flags = CMDFLAGS_COLOUR, CommandColourCustom* with_copy_from = nullptr);
		explicit CommandColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const DirectX::SimpleMath::Color& default_rgba, bool transparency = true, commandflags_t flags = CMDFLAGS_COLOUR, CommandColourCustom* with_copy_from = nullptr);
	};
}
