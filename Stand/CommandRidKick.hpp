#pragma once

#include "CommandAction.hpp"

#include "get_next_arg.hpp"
#include "RemoteGamerConnector.hpp"

namespace Stand
{
	class CommandRidKick : public CommandAction
	{
	public:
		explicit CommandRidKick(CommandList* const parent)
			: CommandAction(parent, LOC("RIDKICK"), CMDNAMES_OBF("ridkick"), NOLABEL, CMDFLAG_FEATURELIST_ULTIMATE)
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [rid]"));
		}

		void onClick(Click& click) final
		{
			if (click.isUltimateEdition())
			{
				CommandAction::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClick(click);
			}

			if (!click.isUltimateEdition())
			{
				return;
			}

			int64_t rid;
			try
			{
				rid = std::stoull(arg);
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
				return;
			}

			ensureYieldableScriptThread([rid]
			{
				RemoteGamerConnector::kickViaRid(rid);
			});
		}
	};
}
