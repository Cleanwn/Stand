#pragma once

#include "CommandTextsliderJoin.hpp"

#include "JoinUtil.hpp"

namespace Stand
{
	class CommandRidSpectate : public CommandTextsliderJoin
	{
	public:
		explicit CommandRidSpectate(CommandList* const parent)
			: CommandTextsliderJoin(parent, LOC("SPEC_R"), CMDNAMES_OBF("ridspectate"))
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": ").append(cmdNameToUtf16(command_names.at(0))).append(L" [rid]"));
		}

		void onClick(Click& click) final
		{
			if (click.isBasicEdition())
			{
				CommandTextsliderJoin::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!click.isBasicEdition())
			{
				return;
			}
			auto arg = get_next_arg(args);
			if (arg.empty())
			{
				return onClick(click);
			}
			try
			{
				JoinUtil::spectate(std::stoull(arg), value);
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
				prefill.push_back(L' ');
				prefill.append(arg);
				click.showCommandBoxIfPossible(std::move(prefill));
			}
		}
	};
}
