#pragma once

#include "CommandInput.hpp"

#include <soup/ObfusString.hpp>

namespace Stand
{
	class CommandInputScrollingText : public CommandInput
	{
	public:
		explicit CommandInputScrollingText(CommandList* const parent)
			: CommandInput(parent, LOC("TEXT"), CMDNAMES("scrollplatetext"), NOLABEL, soup::ObfusString("STAND ON TOP "))
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (args.empty())
			{
				return onClick(click);
			}
			if (!click.isAuto() && args.find(L' ') != std::string::npos && args.back() != L' ')
			{
				args.push_back(L' ');
			}
			onCommandApply(click, args);
		}
	};
}
