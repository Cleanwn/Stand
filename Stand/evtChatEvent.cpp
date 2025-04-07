#include "evtChatEvent.hpp"

#include "AbstractPlayer.hpp"
#include "ChatHistory.hpp"
#include "get_seconds_since_unix_epoch.hpp"

namespace Stand
{
	void evtChatEvent::onConstructed()
	{
		ChatHistory::history.emplace_back(ChatHistory::Message{
			AbstractPlayer(sender).getName(),
			text,
			team_chat,
			is_auto,
			get_seconds_since_unix_epoch()
		});
	}

	std::string evtChatEvent::getLogEntry() const
	{
		std::string log_entry = AbstractPlayer(sender).getName();
		log_entry.append(" [").append(team_chat ? "TEAM" : "ALL").append("] ").append(text);
		return log_entry;
	}
}
