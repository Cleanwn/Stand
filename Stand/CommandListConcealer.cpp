#include "CommandListConcealer.hpp"

#include <algorithm>

namespace Stand
{
	CommandListConcealer::CommandListConcealer(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, commandflags_t flags, CommandType type)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, type)
	{
	}

	void CommandListConcealer::onClick(Click& click)
	{
		if (!wasOrderCaptured())
		{
			for (const auto& child : children)
			{
				if (child->flags & CMDFLAG_CONCEALED)
				{
					++invisible_children;
				}
			}
			if (invisible_children != 0)
			{
				moveChildrenConceal();
			}
		}
		CommandList::onClick(click);
	}

	void CommandListConcealer::resetChildrenWithPreDelete() noexcept
	{
		CommandList::resetChildrenWithPreDelete();
		order.clear();
		invisible_children = 0;
	}

	bool CommandListConcealer::isVisible(Command* const cmd)
	{
		return !cmd->isConcealed();
	}

	void CommandListConcealer::setVisible(Command* const cmd, const bool toggle)
	{
		if (setVisibleBulk(cmd, toggle))
		{
			processChildrenVisibilityUpdate();
		}
	}

	bool CommandListConcealer::setVisibleBulk(Command* const cmd, const bool toggle)
	{
		if (isVisible(cmd) != toggle)
		{
			if (toggle)
			{
				setVisibleRaw(cmd);
			}
			else
			{
				cmd->flags |= CMDFLAG_CONCEALED;
				++invisible_children;
			}
			return true;
		}
		return false;
	}

	void CommandListConcealer::setVisibleRaw(Command* const cmd) noexcept
	{
		cmd->flags &= ~CMDFLAG_CONCEALED;
		--invisible_children;
	}

	void CommandListConcealer::processChildrenVisibilityUpdate()
	{
		moveChildrenConceal();
		processChildrenUpdate();
	}

	void CommandListConcealer::emplaceVisible(std::unique_ptr<Command>&& cmd)
	{
		children.insert(children.end() - invisible_children, std::move(cmd));

		if (invisible_children == 0)
		{
			order.clear();
		}
	}

	bool CommandListConcealer::wasOrderCaptured() const noexcept
	{
		return !order.empty();
	}

	void CommandListConcealer::captureOrder()
	{
		order.reserve(children.size());
		for (const auto& child : children)
		{
			order.emplace_back(child.get());
		}
	}

	size_t CommandListConcealer::getOrderIndex(Command* const cmd) const noexcept
	{
		size_t i = 0;
		for (const auto& item : order)
		{
			if (item == cmd)
			{
				return i;
			}
			++i;
		}
		return -1;
	}

	void CommandListConcealer::moveChildrenAllVisible()
	{
		/*if (!wasOrderCaptured())
		{
			captureOrder();
		}*/

		std::sort(children.begin(), children.end(), [this](const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b)
		{
			return getOrderIndex(a.get()) < getOrderIndex(b.get());
		});
	}

	void CommandListConcealer::moveChildrenConceal()
	{
		if (!wasOrderCaptured())
		{
			captureOrder();
		}

		std::sort(children.begin(), children.end(), [this](const std::unique_ptr<Command>& a, const std::unique_ptr<Command>& b)
		{
			if (!isVisible(a.get()))
			{
				return false;
			}
			if (!isVisible(b.get()))
			{
				return true;
			}
			return getOrderIndex(a.get()) < getOrderIndex(b.get());
		});
	}

	size_t CommandListConcealer::countInvisibleChildren() const noexcept
	{
		return invisible_children;
	}

	size_t CommandListConcealer::countVisibleChildren() const noexcept
	{
		return children.size() - countInvisibleChildren();
	}
}
