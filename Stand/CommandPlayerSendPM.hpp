#pragma once

#include "CommandPlayerAction.hpp"

#include <soup/ObfusString.hpp>

#include "Chat.hpp"
#include "evtChatEvent.hpp"

namespace Stand
{
	class CommandPlayerSendPM : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerSendPM(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("SENDPM"), { CMDNAME("sendpm") }, LOC("SENDPM_H"))
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto str = StringUtils::utf16_to_utf8(args);
			args.clear();
			str.insert(0, fmt::format(fmt::runtime(soup::ObfusString("({}): ").str()), this->getPlayerName()));
			auto target = this->getPlayer();

			Chat::sendTargetedMessageWithReplacements(target, str, true);
			if (g_player != target)
			{
				Chat::sendTargetedMessageWithReplacements(g_player, str, true);
			}

			evtChatEvent::trigger(evtChatEvent(
				g_player,
				str,
				true,
				true,
				false
			));
		}

		std::wstring getCommandSyntax() const final
		{
			std::wstring syntax{ LANG_GET_W("CMD") };
			syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
			syntax.append(LANG_GET_W("ARGTXT"));
			return syntax;
		}
	};
}