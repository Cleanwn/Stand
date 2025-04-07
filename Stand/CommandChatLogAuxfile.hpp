#pragma once

#include "CommandToggle.hpp"

#include "evtChatEvent.hpp"
#include "FileLogger.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandChatLogAuxfile : public CommandToggle
	{
	private:
		static inline FileLogger chat_txt_out{};

		static void onChatEvent(evtChatEvent& e)
		{
			chat_txt_out.log(e.getLogEntry());
		}

	public:
		explicit CommandChatLogAuxfile(CommandList* const parent)
			: CommandToggle(parent, LIT("Chat.txt"))
		{
		}

		void onEnable(Click& click) final
		{
			const auto file_path = std::wstring(_wgetenv(L"appdata")).append(LR"(\Stand\Chat.txt)");
			chat_txt_out.init(file_path);
			if (!chat_txt_out.isInited())
			{
				Util::toast(LANG_FMT("FWERR", StringUtils::utf16_to_utf8(file_path)), TOAST_ALL);
			}
			evtChatEvent::registerHandler(&onChatEvent);
		}

		void onDisable(Click& click) final
		{
			evtChatEvent::unregisterHandler(&onChatEvent);
			chat_txt_out.deinit();
		}
	};
}
