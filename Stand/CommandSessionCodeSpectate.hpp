#pragma once

#include "CommandTextsliderJoin.hpp"

#include "JoinUtil.hpp"

namespace Stand
{
	class CommandSessionCodeSpectate : public CommandTextsliderJoin
	{
	public:
		explicit CommandSessionCodeSpectate(CommandList* parent)
			: CommandTextsliderJoin(parent, LOC("CODESPEC"), { CMDNAME("codespectate") })
		{
		}

		std::wstring getCommandSyntax() const final
		{
			std::wstring syntax{ LANG_GET_W("CMD") };
			syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
			syntax.append(LANG_GET_W("ARGCODE"));
			return syntax;
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (auto code = JoinUtil::parseCode(args); !code.empty())
			{
				JoinUtil::connectViaCode(code, true);
			}
			else
			{
				onClick(click);
			}
		}
	};
}
