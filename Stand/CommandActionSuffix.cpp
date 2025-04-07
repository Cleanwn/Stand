#include "CommandActionSuffix.hpp"

#include "CommandListNameshare.hpp"
#include "lang.hpp"

#ifdef STAND_DEBUG
#include "Exceptional.hpp"
#include "Util.hpp"
#endif

namespace Stand
{
	CommandActionSuffix::CommandActionSuffix(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_suffixes, Label&& help_text, commandflags_t flags, const CommandPerm perm, CommandType type)
		: CommandAction(parent, std::move(menu_name), {}, std::move(help_text), flags, perm, {}, type), command_suffixes(std::move(command_suffixes))
	{
		CommandList* list = parent;
		while (!list->isListNameshare())
		{
			list = list->parent;
#ifdef STAND_DEBUG
			if (list == nullptr)
			{
				Exceptional::report("CommandActionSuffix must have a CommandListNameshare somewhere up the tree");
				return;
			}
#endif
		}

#ifdef STAND_DEBUG
		if (!command_suffixes.empty() && list->command_names.empty())
		{
			Util::toast(std::move(list->getPathEn().append(" has no command name, you idiot!")), TOAST_ALL);
		}
#endif

		setCommandNames(list->as<CommandListNameshare>());
	}

	void CommandActionSuffix::setCommandNames(CommandListNameshare* base)
	{
		for (const auto& command_suffix : this->command_suffixes)
		{
			for (const auto& command_name : base->command_names)
			{
				command_names.emplace_back(std::move(CommandName(command_name).append(command_suffix)));
			}
		}
	}

	void CommandActionSuffix::updateCommandNames()
	{
		command_names.clear();
		setCommandNames(getListNameshare());
	}

	CommandListNameshare* CommandActionSuffix::getListNameshare() const
	{
		CommandList* list = parent;
		while (!list->isListNameshare())
		{
			list = list->parent;
		}
		return list->as<CommandListNameshare>();
	}

	std::wstring CommandActionSuffix::getCommandSyntax() const
	{
		if (command_suffixes.empty())
		{
			return {};
		}
		CommandListNameshare* const base = getListNameshare();
		if (base->command_names.empty())
		{
			return {};
		}
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(base->command_names.at(0))).push_back(L' ');
		syntax.append(cmdNameToUtf16(command_suffixes.at(0)));
		return syntax;
	}
}
