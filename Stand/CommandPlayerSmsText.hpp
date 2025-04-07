#pragma once

#include "CommandInputText.hpp"

#include "CommandExtraInfo.hpp"

namespace Stand
{
	class CommandPlayerSmsText : public CommandOnPlayer<CommandInputText>
	{
	public:
		explicit CommandPlayerSmsText(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("TEXT"), CMDNAMES_OBF("smstext"), NOLABEL, soup::ObfusString("Pizza Time").str())
		{
			PC_PTR->registerCommand(this);
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			CommandOnPlayer::getExtraInfo(info, args);
			info.char_limit = 255;
			info.colour_selector = true;
		}
	};
}
