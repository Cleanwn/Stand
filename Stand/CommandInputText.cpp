#include "CommandInputText.hpp"

#include "lang.hpp"

namespace Stand
{
	std::wstring CommandInputText::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
		syntax.append(LANG_GET_W("ARGTXT"));
		return syntax;
	}
}
