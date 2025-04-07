#pragma once

#include "CommandAction.hpp"

#include "lang.hpp"

namespace Stand
{
	class CommandActionName : public CommandAction
	{
	public:
		using CommandAction::CommandAction;

		std::wstring getCommandSyntax() const final
		{
			std::wstring syntax{ LANG_GET_W("CMD") };
			syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
			syntax.append(LANG_GET_W("ARGNME"));
			return syntax;
		}
	};
}
