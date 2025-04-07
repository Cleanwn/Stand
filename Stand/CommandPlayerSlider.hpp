#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandSlider.hpp"

namespace Stand
{
	class CommandPlayerSlider : public CommandOnPlayer<CommandSlider>
	{
	public:
		explicit CommandPlayerSlider(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const int min_value, const int max_value, const int default_value, const unsigned int step_size = 1, const commandflags_t flags = CMDFLAGS_SLIDER, const CommandPerm perm = COMMANDPERM_USERONLY, bool is_click_to_apply = false)
			: CommandOnPlayer(parent, std::move(menu_name), std::move(command_names), std::move(help_text), min_value, max_value, default_value, step_size, flags, perm, is_click_to_apply, {}, COMMAND_SLIDER)
		{
			PC_PTR->registerCommandNames(this);
		}
	};
}