#pragma once

namespace Stand
{
	struct ChatHistory
	{
		struct Message
		{
			std::string sender_name;
			std::string contents;
			bool team_chat;
			bool is_auto;
			time_t time; // 0 if message was obtained from scaleform
		};

		inline static std::vector<Message> history{}; // chronological order

		static void fromScaleform();
	};
}
