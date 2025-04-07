#include "CommandListUserkv.hpp"

#include "extract_delimited_string_with_escaping.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandListUserkv::CommandListUserkv(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::string&& default_state)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), CMDFLAGS_LIST | CMDFLAG_SUPPORTS_STATE_OPERATIONS), default_state(std::move(default_state))
	{
	}

	std::string CommandListUserkv::getState() const
	{
		std::string state{};
		for (const auto& entry : entries)
		{
			state.append(entry.first).push_back('<');
			std::string value = getValue(entry.first, entry.second);
			StringUtils::replace_all(value, "\\", "\\\\");
			StringUtils::replace_all(value, ">", "\\>");
			state.append(std::move(value)).push_back('>');
		}
		return state;
	}

	std::string CommandListUserkv::getDefaultState() const
	{
		return default_state;
	}

	void CommandListUserkv::setState(Click& click, const std::string& state)
	{
		if (!children.empty())
		{
			for (auto i = children.begin() + body_offset; i != children.end(); ++i)
			{
				onRemoveItem(i->get()->as<CommandPhysical>());
			}
			entries.clear();
			resetChildrenWithPreDelete();
		}
		populateHead();
		body_offset = children.size();
		std::string state_ = state;
		size_t del_pos;
		while ((del_pos = state_.find('<')) != std::string::npos)
		{
			std::string key = state_.substr(0, del_pos);
			state_.erase(0, del_pos + 1);
			addItem(std::move(key), extract_delimited_string_with_escaping(state_, '>'));
		}
		processChildrenUpdate();
	}

	void CommandListUserkv::applyDefaultState()
	{
		Click click(CLICK_BULK, TC_APPLYDEFAULTSTATE);
		setState(click, default_state);
	}

	void CommandListUserkv::addItem(ThreadContext thread_context, std::string&& key, std::string&& value)
	{
		auto entry = find(key);
		if (entry == entries.end())
		{
			focusItem(thread_context, addItem(std::move(key), std::move(value)));
			g_gui.on_command_state_change();
		}
		else
		{
			focusItem(thread_context, entry->second);
		}
	}

	CommandPhysical* CommandListUserkv::addItem(std::string&& key, std::string&& value)
	{
		auto cmd = emplaceItem(key, std::move(value));
		entries.emplace_back(std::move(key), cmd);
		return cmd;
	}

	void CommandListUserkv::removeItem(const std::string& key)
	{
		auto entry = find(key);
		// entry must exist
		onRemoveItem(entry->second);
		entry->second->beGone();
		entries.erase(entry);
	}

	std::vector<std::pair<std::string, CommandPhysical*>>::iterator CommandListUserkv::find(const std::string& key)
	{
		auto i = entries.begin();
		while (i != entries.end() && i->first != key)
		{
			++i;
		}
		return i;
	}
}
