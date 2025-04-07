#include "CommandListReadonly.hpp"

namespace Stand
{
	CommandListReadonly::CommandListReadonly(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, COMMAND_LIST_READONLY)
	{
	}

	void CommandListReadonly::doInitialTick()
	{
		had_initial_tick = true;
		for (const auto& child : children)
		{
			if (child->type == COMMAND_READONLY_VALUE)
			{
				((CommandPhysical*)child.get())->onTickInGameViewport();
			}
		}
	}

	void CommandListReadonly::onActiveListUpdate()
	{
		if (isCurrentUiOrWebList())
		{
			had_initial_tick = false;
			ensureScriptThread([this]
			{
				doInitialTick();
			});
		}
	}
}
