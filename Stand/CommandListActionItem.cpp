#include "CommandListActionItem.hpp"

#include "CommandListAction.hpp"

namespace Stand
{
	CommandListActionItem::CommandListActionItem(CommandList* const parent, long long value, Label&& menu_name, const std::vector<CommandName>& command_suffixes, Label&& help_text, commandflags_t flags, const CommandPerm perm)
		: CommandActionSuffix(parent, std::move(menu_name), std::vector<CommandName>(command_suffixes), std::move(help_text), flags, perm, COMMAND_ACTION_ITEM), value(value)
	{
	}

	CommandListAction* CommandListActionItem::getListAction() const
	{
		CommandList* list = parent;
		while (!list->isListAction())
		{
			list = list->parent;
		}
		return list->as<CommandListAction>();
	}

	void CommandListActionItem::onClick(Click& click)
	{
		if (!parent->job_queued)
		{
			getListAction()->onItemClick(click, this);
		}
	}
}
