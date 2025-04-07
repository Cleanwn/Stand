#include "CommandListActionCategory.hpp"

#include "CommandListActionItem.hpp"
#include "CommandListSelect.hpp"

namespace Stand
{
	void CommandListActionCategory::onClick(Click& click)
	{
		if (!click.isWeb())
		{
			CommandList::onClick(click);

			if (parent->isT<CommandListSelect>()) // Stateful list action?
			{
				if (auto cmd = parent->as<CommandListSelect>()->getCurrentValueCommand())
				{
					if (cmd->parent == this) // Current value is in this category?
					{
						cmd->focusInParent(click.thread_context); // Focus command for current value
					}
				}
			}
		}
	}
}
