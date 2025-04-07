#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListReadonly : public CommandList
	{
	public:
		bool had_initial_tick = false;

		explicit CommandListReadonly(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = CMDFLAGS_LIST);

		void doInitialTick();

		void onActiveListUpdate() final;
	};
}
