#include "CommandColour.hpp"

namespace Stand
{
	CommandColour::CommandColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int default_r, int default_g, int default_b, int default_a, commandflags_t flags, CommandColourCustom* with_copy_from)
		: CommandColourCustom(parent, std::move(menu_name), std::move(command_names), std::move(help_text), default_r, default_g, default_b, default_a, flags)
	{
		populateWithOptionalCopyFrom(with_copy_from);
	}

	CommandColour::CommandColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const DirectX::SimpleMath::Color& default_rgba, bool transparency, commandflags_t flags, CommandColourCustom* with_copy_from)
		: CommandColour(parent, std::move(menu_name), std::move(command_names), std::move(help_text), (int)(default_rgba.x * 255.0f), (int)(default_rgba.y * 255.0f), (int)(default_rgba.z * 255.0f), transparency ? (int)(default_rgba.w * 255.0f) : -1, flags, with_copy_from)
	{
	}
}
