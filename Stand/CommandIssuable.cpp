#include "CommandIssuable.hpp"

#include "CommandExtraInfo.hpp"
#include "CommandList.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"

#ifdef STAND_DEBUG
#include <fmt/core.h>

#include "Util.hpp"
#endif

namespace Stand
{
#if COMPACT_COMMAND_NAMES
	std::wstring cmdNameToUtf16(const std::string& str)
#else
	std::wstring utf8ToCmdName(const std::string& str)
#endif
	{
		return StringUtils::utf8_to_utf16(str);
	}

#if !COMPACT_COMMAND_NAMES
	std::string cmdNameToUtf8(const std::wstring& str)
	{
		return StringUtils::utf16_to_utf8(str);
	}
#endif

	CommandIssuable::CommandIssuable(CommandList* parent, std::vector<CommandName>&& command_names, CommandPerm perm, commandflags_t flags, CommandType type)
		: Command(type, parent, flags), command_names(std::move(command_names)), perm(perm)
	{
#ifdef STAND_DEBUG
		if (parent != nullptr
			&& getRoot() == g_gui.root_list.get()
			)
		{
			for (const auto& command_name : this->command_names)
			{
				CommandName simplified_command_name = command_name;
				StringUtils::simplifyCommandName(simplified_command_name);
				if (simplified_command_name != command_name)
				{
					Util::toast(fmt::format("{} is not simple, therefore it will not be executable!", cmdNameToUtf8(command_name)), TOAST_ALL);
				}
			}
		}
#endif
	}

	bool CommandIssuable::mustHaveCommandName() const
	{
#ifdef STAND_DEBUG
		if (this->command_names.empty())
		{
			Util::toast(std::move(getPathEn().append(" doesn't have a command name, you idiot!")), TOAST_ALL);
			return true;
		}
#endif
		return false;
	}

	CommandList* CommandIssuable::getRoot()
	{
		CommandList* list = isList() ? (CommandList*)this : parent;
		while (!list->isRoot())
		{
			list = list->parent;
		}
		return list;
	}

	bool CommandIssuable::isTab() const
	{
		return parent == g_gui.root_list.get();
	}

	std::wstring CommandIssuable::getActivationNameLocalisedUtf16() const
	{
		if (isPhysical())
		{
			return Command::getPhysical()->getActivationName().getLocalisedUtf16();
		}
		if (!command_names.empty())
		{
			return cmdNameToUtf16(command_names.at(0));
		}
		return L"{}";
	}

	std::string CommandIssuable::getActivationNameLocalisedUtf8() const
	{
		return StringUtils::utf16_to_utf8(getActivationNameLocalisedUtf16());
	}

	void CommandIssuable::addSuffixToCommandNames(CommandName&& suffix)
	{
		StringUtils::simplify(suffix);
		addSuffixToCommandNamesSimple(suffix);
	}

	void CommandIssuable::addSuffixToCommandNamesSimple(const CommandName& suffix)
	{
		for (auto& command_name : command_names)
		{
			command_name.append(suffix);
		}
	}

	std::vector<CommandName> CommandIssuable::getPrefixedCommandNames(const CommandName& prefix) const
	{
		std::vector<CommandName> res{};
		res.reserve(command_names.size());
		for (const auto& command_name : command_names)
		{
			res.emplace_back(std::move(CommandName(prefix).append(command_name)));
		}
		return res;
	}

	std::vector<CommandName> CommandIssuable::getSuffixedCommandNames(const CommandName& suffix) const
	{
		return combineCommandNames(command_names, suffix);
	}

	std::vector<CommandName> CommandIssuable::getSuffixedCommandNames(const std::vector<CommandName>& suffixes) const
	{
		return combineCommandNames(command_names, suffixes);
	}

	void CommandIssuable::combineCommandNames(std::vector<CommandName>& out_command_names, const std::vector<CommandName>& prefixes, const std::vector<CommandName>& suffixes)
	{
		for (const auto& prefix : prefixes)
		{
			for (const auto& suffix : suffixes)
			{
				out_command_names.emplace_back(std::move(CommandName(prefix).append(suffix)));
			}
		}
	}

	std::vector<CommandName> CommandIssuable::combineCommandNames(const std::vector<CommandName>& prefixes, const std::vector<CommandName>& suffixes)
	{
		std::vector<CommandName> res{};
		res.reserve(prefixes.size() * suffixes.size());
		combineCommandNames(res, prefixes, suffixes);
		return res;
	}

	std::vector<CommandName> CommandIssuable::combineCommandNames(const std::vector<CommandName>& prefixes, const CommandName& suffix)
	{
		std::vector<CommandName> res{};
		res.reserve(prefixes.size());
		for (const auto& prefix : prefixes)
		{
			res.emplace_back(std::move(CommandName(prefix).append(suffix)));
		}
		return res;
	}

	std::vector<CommandName> CommandIssuable::combineCommandNames(const CommandName& prefix, const std::vector<CommandName>& suffixes)
	{
		std::vector<CommandName> res{};
		res.reserve(suffixes.size());
		for (const auto& suffix : suffixes)
		{
			res.emplace_back(std::move(CommandName(prefix).append(suffix)));
		}
		return res;
	}

	bool CommandIssuable::isAuthorised(const CommandPerm perm) const
	{
		return (this->perm & perm) != 0;
	}

	bool CommandIssuable::isAuthorised(const Click& click) const
	{
		return isAuthorised(click.getEffectivePermissions());
	}

	std::wstring CommandIssuable::getCompletionHint() const
	{
		std::wstring hint{ cmdNameToUtf16(command_names.at(0)) };
		if (isPhysical())
		{
			hint.append(L" - ");
			hint.append(((const CommandPhysical*)this)->getActivationName().getLocalisedUtf16());
			//hint.append(StringUtils::utf8_to_utf16(getPathEn()));
		}
		return hint;
	}

	void CommandIssuable::getExtraInfo(CommandExtraInfo& info, std::wstring& args)
	{
		info.completed_hint = cmdNameToUtf16(command_names.at(0));
	}
}
