#include "ChatCommandsCommon.hpp"

#include "CommandListSelect.hpp"
#include "CommandPlayer.hpp"
#include "evtChatEvent.hpp"
#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	const wchar_t ChatCommandsCommon::getEffectiveChatPrefix()
	{
		switch (prefix->value)
		{
		default:
		case 0:
			return L'!';
		case 1:
			return L'?';
		case 2:
			return L';';
		case 3:
			return L',';
		case 4:
			return L'.';
		case 5:
			return L'+';
		case 6:
			return L'-';
		case 7:
			return L'*';
		case 8:
			return L'/';
		case 9:
			return L'\\';
		case 10:
			return L'%';
		case 11:
			return L'=';
		case 12:
			return L'#';
		case 13:
			return L'$';
		case 14:
			return L'€';
		}
	}

	const char* ChatCommandsCommon::getEffectiveReplyPrefix()
	{
		switch (reply_prefix->value)
		{
		case 0:
			return "";
		default:
		//case 1:
			//return "> ";
		case 2:
			return "! ";
		case 3:
			return "? ";
		case 4:
			return "; ";
		case 5:
			return ", ";
		case 6:
			return ". ";
		case 7:
			return "+ ";
		case 8:
			return "- ";
		case 9:
			return "* ";
		case 10:
			return "/ ";
		case 11:
			return "\\ ";
		case 12:
			return "% ";
		case 13:
			return "= ";
		case 14:
			return "# ";
		case 15:
			return "$ ";
		case 16:
			return "€ ";
		}
	}

	static std::array<time_t, MAX_PLAYERS> last_command_issuance;

	void ChatCommandsCommon::processMessageFromSenderWithChatCommandPermissions(evtChatEvent& e)
	{
		if (e.is_auto
			|| (e.handled_flags & evtChatEvent::HANDLED_CHAT_COMMAND)
			)
		{
			return;
		}
		e.handled_flags |= evtChatEvent::HANDLED_CHAT_COMMAND;

		if (GET_MILLIS_SINCE(last_command_issuance.at(e.sender)) < 500)
		{
			if (auto cmd = AbstractPlayer(e.sender).getCommand())
			{
				cmd->aggressive_action_warranted = true;
			}
			return;
		}
		last_command_issuance.at(e.sender) = get_current_time_millis();

		auto cmd = StringUtils::utf8_to_utf16(e.text);
		if (cmd.length() > 2
			&& cmd.at(0) == getEffectiveChatPrefix()
			&& cmd.at(1) != L' ' // Don't react to chat command replies since is_auto is no longer networked
			)
		{
			cmd.erase(cmd.begin());
			const compactplayer_t sender_player_id = e.sender;
			const bool team_chat = e.team_chat;
			FiberPool::queueJob([cmd{ std::move(cmd) }, sender_player_id, team_chat]() mutable
			{
				Click click(team_chat ? CLICK_CHAT_TEAM : CLICK_CHAT_ALL, TC_SCRIPT_YIELDABLE, sender_player_id);
				g_gui.triggerCommands(std::move(cmd), click);
			});
		}
	}
}
