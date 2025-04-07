#include "CommandListNameshare.hpp"

#include <fmt/core.h>

#include "CommandExtraInfo.hpp"
#include "get_next_arg.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandListNameshare::CommandListNameshare(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, const commandflags_t flags, const CommandType type)
		: CommandList(parent, std::move(menu_name), std::move(command_names), std::move(help_text), flags, type)
	{
	}

	std::vector<CommandPhysical*> CommandListNameshare::processArgs(CommandPerm perms, std::wstring& args)
	{
		if (!command_names.empty()) // Neeed to avoid out-of-range when lua script does trigger_command on us
		{
			auto og_args = args;
			auto arg = get_next_arg(args);
			StringUtils::simplify(arg);
			if (!arg.empty())
			{
				arg.insert(0, cmdNameToUtf16(command_names.at(0)));
				std::vector<CommandPhysical*> results{};
#if COMPACT_COMMAND_NAMES
				CommandName command_name_prefix = StringUtils::utf16_to_utf8(arg);
#else
				const CommandName& command_name_prefix = arg;
#endif
				findCommandsWhereCommandNameStartsWith(results, command_name_prefix, perms);
				if (!results.empty())
				{
					return results;
				}
			}
			args = std::move(og_args);
		}
		return { this };
	}

	void CommandListNameshare::onCommand(Click& click, std::wstring& args)
	{
		const auto perms = click.getEffectivePermissions();
		CommandPhysical* cmd = this;
		auto cmds = processArgs(perms, args);
		if (cmds.size() == 1)
		{
			cmd = cmds.at(0);
		}
		if (cmd->isAuthorised(perms))
		{
			return cmd->onClick(click);
		}
		click.setResponse(LIT(fmt::format(fmt::runtime(Lang::get(cmd->perm == COMMANDPERM_USERONLY ? ATSTRINGHASH("CMDISS") : ATSTRINGHASH("CMDPERM"))), cmd->getActivationName().getLocalisedUtf8())));
		click.respond();
		click.forgetResponse();
	}

	void CommandListNameshare::getExtraInfo(CommandExtraInfo& info, std::wstring& args)
	{
		auto cmds = processArgs(COMMANDPERM_USERONLY, args);
		if (cmds.size() == 1)
		{
			auto cmd = cmds.at(0);
			if (cmd != this)
			{
				std::wstring args_{};
				return cmd->getExtraInfo(info, args_);
			}
		}
		else
		{
			info.collapse = true;
		}
		return CommandList::getExtraInfo(info, args);
	}
}
