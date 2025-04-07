#include "CommandChatSing.hpp"

#include <filesystem>
#include <fstream>

#include "Chat.hpp"
#include "FiberPool.hpp"
#include "regular_event.hpp"
#include "TimedTextPresenter.hpp"

namespace Stand
{
	static constexpr const char* default_song = R"([00:18.82]We are no strangers to love
[00:22.81]You know the rules and so do I
[00:26.91]A full commitment's what I am thinking of
[00:30.91]You wouldn't get this from any other guy
[00:34.90]I just want to tell you how I am feeling
[00:39.82]Gotta make you understand
[00:43.32]Never gonna give you up, never gonna let you down
[00:47.11]Never gonna run around and desert you
[00:51.61]Never gonna make you cry, never gonna say goodbye
[00:55.52]Never gonna tell a lie and hurt you
[01:00.62]We've known each other for so long
[01:04.61]Your heart's been aching but You are too shy to say it
[01:09.01]Inside we both know what's been going on
[01:13.11]We know the game and We are gonna play it
[01:17.31]And if you ask me how I am feeling
[01:22.80]Don't tell me You are too blind to see
[01:25.52]Never gonna give you up, never gonna let you down
[01:29.21]Never gonna run around and desert you
[01:33.91]Never gonna make you cry, never gonna say goodbye
[01:37.71]Never gonna tell a lie and hurt you
[01:42.02]Never gonna give you up, never gonna let you down
[01:46.22]Never gonna run around and desert you
[01:50.81]Never gonna make you cry, never gonna say goodbye
[01:54.70]Never gonna tell a lie and hurt you
[01:59.70](Ooh give you up)
[02:03.70](Ooh give you up)
[02:08.31]Never gonna give, never gonna give (give you up)
[02:12.10]Never gonna give, never gonna give (give you up)
[02:16.31]We've known each other for so long
[02:20.70]Your heart's been aching but You are too shy to say it
[02:25.01]Inside we both know what's been going on
[02:29.22]We know the game and We are gonna play it
[02:33.41]I just want to tell you how I am feeling
[02:37.81]Gotta make you understand
[02:41.31]Never gonna give you up, never gonna let you down
[02:45.32]Never gonna run around and desert you
[02:49.51]Never gonna make you cry, never gonna say goodbye
[02:53.91]Never gonna tell a lie and hurt you
[02:58.00]Never gonna give you up, never gonna let you down
[03:02.20]Never gonna run around and desert you
[03:06.32]Never gonna make you cry, never gonna say goodbye
[03:10.60]Never gonna tell a lie and hurt you
[03:15.31]Never gonna give you up, never gonna let you down
[03:19.00]Never gonna run around and desert you
[03:23.41]Never gonna make you cry, never gonna say goodbye
[03:27.51]Never gonna tell a lie and hurt you)";

	CommandChatSing::CommandChatSing(CommandList* parent)
		: CommandToggleRegular(parent, LOC("CHATSING"), CMDNAMES("singing", "chatsing"), LOC("CHATSING_H"))
	{
		auto file = std::move(std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Song.lrc)"));
		if (!std::filesystem::exists(file))
		{
			std::ofstream f(file);
			f.write(default_song, strlen(default_song));
		}
	}

	static TimedTextPresenter ttp{};
	static time_t last_offset_millis;
	static void onTick()
	{
		auto cur_text = ttp.getCurrentText();
		if (cur_text.offset_millis != last_offset_millis)
		{
			last_offset_millis = cur_text.offset_millis;
			auto str = cur_text.text;
			FiberPool::queueJob([str{std::move(str)}]
			{
				Chat::sendMessage(std::move(str), false, true, true);
			});
		}
	}

	void CommandChatSing::onEnableInner(Click& click)
	{
		std::ifstream f(std::move(std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Song.lrc)")));
		auto c = TimedTextCollection::fromLRC(f);
		c.eraseAdvertisements();
		c.setStartOffset(0);
		last_offset_millis = -1;
		ttp.startPresenting(std::move(c));
		reScriptTickEvent::registerHandler(&onTick);
	}

	void CommandChatSing::onDisableInner(Click& click)
	{
		reScriptTickEvent::unregisterHandler(&onTick);
		ttp.presentation.entries.clear();
	}
}
