#include "CommandListRefreshable.hpp"

#include <algorithm>

#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"

namespace Stand
{
	CommandListRefreshable::CommandListRefreshable(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), 0, COMMAND_LIST_REFRESHABLE)
	{
	}

	void CommandListRefreshable::populate()
	{
		populateHead();
		body_offset = children.size();
		populateBody();
		if (children.size() > body_offset)
		{
			createDivider();
		}
	}

	void CommandListRefreshable::onClick(Click& click)
	{
		refresh([this, click]
		{
			active = true;
			Click click_(click);
			click_.thread_context = TC_WORKER;
			CommandList::onClick(click_);
		});
	}

	void CommandListRefreshable::onActiveListUpdate()
	{
		if (isThisOrSublistCurrentMenuListInTab())
		{
			if (!active)
			{
				active = true;
				if (isThisOrSublistCurrentUiListInTab())
				{
					refresh();
				}
			}
		}
		else
		{
			active = false;
		}
	}

	void CommandListRefreshable::refresh(std::function<void()>&& callback)
	{
		FiberPool::queueJob([this, callback{ std::move(callback) }]
		{
			EXCEPTIONAL_LOCK_WRITE(g_gui.root_mtx)
			auto body_offset_with_divider = getFullBodyOffset();
			if (children.size() >= body_offset_with_divider)
			{
				for (auto i = children.begin() + body_offset_with_divider; i != children.end(); )
				{
					processChild(this, i);
				}
			}
			populateBody();
			if (children.size() > body_offset_with_divider)
			{
				if (!divider)
				{
					createDivider();
				}
				sortImpl();
			}
			else if (divider)
			{
				divider->beGone();
				divider = nullptr;
			}
			EXCEPTIONAL_UNLOCK_WRITE(g_gui.root_mtx)
			updateWebState();
			callback();
			processChildrenUpdate();
			for (auto i = children.cbegin() + getFullBodyOffset(); i != children.cend(); ++i)
			{
				if ((*i)->type == COMMAND_LIST)
				{
					((CommandList*)i->get())->processChildrenUpdate();
				}
			}
		});
	}

	void CommandListRefreshable::processChild(CommandList* list, std::vector<std::unique_ptr<Command>>::iterator& i) const
	{
		if (shouldDelete(i->get()))
		{
			if ((*i)->isPhysical())
			{
				((CommandPhysical*)i->get())->applyDefaultState();
			}
			(*i)->preDelete();
			i = list->children.erase(i);
		}
		else
		{
			onItemIsKept(i->get());
			++i;
		}
	}

	void CommandListRefreshable::createDivider()
	{
		if (body_offset != 0)
		{
			divider = (CommandDivider*)children.emplace(children.begin() + body_offset, makeChild<CommandDivider>(Label(menu_name)))->get();
		}
	}

	void CommandListRefreshable::sort()
	{
		sortImpl();
		processChildrenUpdate();
	}

	void CommandListRefreshable::sortImpl()
	{
		return sortBody(&sortAlgoName);
	}

	void CommandListRefreshable::sortBody(sort_algo_t algo)
	{
		std::sort(children.begin() + getFullBodyOffset(), children.end(), algo);
	}

	void CommandListRefreshable::sortBodyRecursively(sort_algo_t algo)
	{
		recursivelySort(children.begin() + getFullBodyOffset(), children.end(), algo);
	}

	void CommandListRefreshable::recursivelySort(std::vector<std::unique_ptr<Command>>::iterator begin, std::vector<std::unique_ptr<Command>>::iterator end, sort_algo_t algo)
	{
		std::sort(begin, end, algo);
		for (auto i = begin; i != end; ++i)
		{
			if (isSubFolder(i->get()))
			{
				auto* list = (CommandList*)i->get();
				recursivelySort(list->children.begin(), list->children.end(), algo);
			}
		}
	}

	bool CommandListRefreshable::isSubFolder(Command* cmd) noexcept
	{
		return cmd->type == COMMAND_LIST
			&& cmd->as<CommandList>()->flags == (CMDFLAGS_LIST | CMDFLAG_SEARCH_CHILDRENONLY | CMDFLAG_FEATURELIST_SKIP);
	}

	void CommandListRefreshable::populateHead()
	{
	}

	bool CommandListRefreshable::shouldDelete(Command* cmd) const
	{
		return true;
	}

	void CommandListRefreshable::onItemIsKept(Command* cmd) const
	{
	}

	size_t CommandListRefreshable::getFullBodyOffset() const
	{
		size_t body_offset_with_divider = body_offset;
		if (divider)
		{
			++body_offset_with_divider;
		}
		return body_offset_with_divider;
	}
}
