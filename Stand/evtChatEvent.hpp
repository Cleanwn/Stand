#pragma once

#include "evtEvent.hpp"

#include <string>

#include "gta_player.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct evtChatEvent : public evtEvent<evtChatEvent>
	{
		using HandledFlags_t = uint8_t;
		enum HandledFlags : HandledFlags_t
		{
			HANDLED_CHAT_COMMAND = 1 << 0,
		};

		const compactplayer_t sender;
		const std::string text;
		const bool team_chat;
		const bool networked;
		const bool is_auto;
		HandledFlags_t handled_flags = 0;

		explicit evtChatEvent(const compactplayer_t sender, std::string text, const bool team_chat, const bool networked, const bool is_auto)
			: sender(sender), text(std::move(text)), team_chat(team_chat), networked(networked), is_auto(is_auto)
		{
			onConstructed();
		}

		[[nodiscard]] std::string getLogEntry() const;

	private:
		void onConstructed();
	};
#pragma pack(pop)
}
