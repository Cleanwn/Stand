#pragma once

#include "CommandAction.hpp"

#include "natives.hpp"

namespace Stand
{
	class CommandRidInvite : public CommandAction
	{
	public:
		explicit CommandRidInvite(CommandList* const parent)
			: CommandAction(parent, LOC("INV_R"), CMDNAMES_OBF("ridinvite"))
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [rid]"));
		}

		void onClick(Click& click) final
		{
			if (JoinUtil::inviteViaRidPreflightCheck(click))
			{
				return CommandAction::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!JoinUtil::inviteViaRidPreflightCheck(click))
			{
				return;
			}
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClick(click);
			}
			int64_t rid = 0;
			try
			{
				rid = std::stoull(arg);
			}
			catch (const std::exception&)
			{
			}
			if (rid == 0)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
				return;
			}
			ensureYieldableScriptThread(click, [rid]
			{
				JoinUtil::inviteViaRid(rid);
			});
		}
	};
}
