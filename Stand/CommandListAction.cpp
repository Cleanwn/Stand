#include "CommandListAction.hpp"

#include <bitset>

#include "CommandListActionCategory.hpp"
#include "CommandListActionItem.hpp"
#include "CommandListActionItemWithEntityPreview.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandListAction::CommandListAction(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::vector<CommandListActionItemData>&& options, const commandflags_t flags, const commandflags_t item_flags, const CommandType type)
		: CommandListNameshare(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, type), item_flags(item_flags), options(std::move(options))
	{
		populate();
	}

	void CommandListAction::setOptions(std::vector<CommandListActionItemData>&& options)
	{
		EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
		this->options = std::move(options);
		children.clear();
		populate();
		EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
	}

	void CommandListAction::setOptions(const std::vector<CommandListActionItemData>& options)
	{
		setOptions(std::vector<CommandListActionItemData>(options));
	}

	CommandList* CommandListAction::getCategory(const Label& name)
	{
		for (const auto& child : this->children)
		{
			if (child->isT<CommandList>())
			{
				auto list = child->as<CommandList>();
				if (list->menu_name == name)
				{
					return list;
				}
			}
		}
		return (CommandList*)this->createChild<CommandListActionCategory>(name);
	}

	void CommandListAction::populate()
	{
		CommandPerm child_perm = COMMANDPERM_USERONLY;
		Label current_category_label = NOLABEL;
		CommandList* current_category_list = this;

		for (const auto& option : options)
		{
			if (option.perm != COMMANDPERM_USERONLY)
			{
				child_perm |= option.perm;
			}

			if (current_category_label != option.category)
			{
				current_category_label = option.category;
				current_category_list = option.category.empty()
					? this
					: getCategory(option.category)
					;
			}

			if (option.entity_preview)
			{
				current_category_list->createChild<CommandListActionItemWithEntityPreview>(option.value, Label(option.menu_name), option.command_suffixes, Label(option.help_text), item_flags | option.flags, option.perm);
			}
			else
			{
				current_category_list->createChild<CommandListActionItem>(option.value, Label(option.menu_name), option.command_suffixes, Label(option.help_text), item_flags | option.flags, option.perm);
			}
		}

		// Update my permission if children all have the same non-useronly permission
		if (child_perm != COMMANDPERM_USERONLY)
		{
			child_perm &= ~COMMANDPERM_USERONLY;
			if(std::bitset<sizeof(CommandPerm) * 8>(child_perm).count() == 1)
			{
				this->perm = child_perm;
			}
			else
			{
				this->perm = COMMANDPERM_ANY;
			}
		}
	}

	void CommandListAction::updateCommandNamesOfChildren()
	{
		// At this point, the command was not attached to the tree yet, so it's fine to call the root readlocked variant.
		recursiveForEachNonListChildRootReadLocked([](Command* cmd)
		{
			cmd->as<CommandListActionItem>()->updateCommandNames();
		});
	}

	void CommandListAction::onItemClick(Click& click, CommandListActionItem* item)
	{
		click.stopInputIfAllowed();

		// Put the cursor on this list, not in it
		auto ui_list = g_gui.getCurrentUiList();
		while (isThisOrSublist(ui_list))
		{
			ui_list->goBackIfActive(click.thread_context);
			ui_list = ui_list->parent;
		}
	}

	const CommandListActionItemData* CommandListAction::getValueData(long long value) const
	{
		for (const auto& option : options)
		{
			if (option.value == value)
			{
				return &option;
			}
		}
		return nullptr;
	}

	[[nodiscard]] static CommandListActionItem* getValueCommandImpl(const CommandList* list, long long value)
	{
		for (const auto& cmd : list->children)
		{
			if (cmd->type == COMMAND_LIST)
			{
				if (auto res = getValueCommandImpl(cmd->as<CommandList>(), value))
				{
					return res;
				}
			}
			else
			{
				if (cmd->as<CommandListActionItem>()->value == value)
				{
					return cmd->as<CommandListActionItem>();
				}
			}
		}
		return nullptr;
	}

	CommandListActionItem* CommandListAction::getValueCommand(long long value) const
	{
		return getValueCommandImpl(this, value);
	}

	Label CommandListAction::getValueMenuName(long long value) const
	{
		if (auto data = getValueData(value))
		{
			return data->menu_name;
		}
		return NOLABEL;
	}
}
