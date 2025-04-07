#pragma once

#include "CommandListAction.hpp"

namespace Stand
{
	class CommandListActionLambda : public CommandListAction
	{
	private:
		std::function<void(Click&, CommandListActionItem*)> onItemClickImpl;

	public:
		explicit CommandListActionLambda(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandListActionItemData>&& options, std::function<void(Click&, CommandListActionItem*)>&& onItemClickImpl)
			: CommandListAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), std::move(options)), onItemClickImpl(std::move(onItemClickImpl))
		{
		}

		void onItemClick(Click& click, CommandListActionItem* item) final
		{
			onItemClickImpl(click, item);
		}
	};
}
