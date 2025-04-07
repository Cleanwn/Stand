#pragma once

#include "CommandToggle.hpp"

#include "Chat.hpp"
#include "Commandbox.hpp"
#include "tbScreenshotMode.hpp"

namespace Stand
{
	class CommandChatAlwaysOpen : public CommandToggle
	{
	public:
		explicit CommandChatAlwaysOpen(CommandList* const parent)
			: CommandToggle(parent, LOC("CHATAO"), CMDNAMES_OBF("chatalwaysopen", "keepchatopen"), NOLABEL)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);
				if (is_session_started())
				{
					if (!Chat::isOpen()
						&& !g_tb_screenshot_mode.isEnabled()
						)
					{
						pointers::CMultiplayerChat_SetFocus(*pointers::chat_box, 1, 0);
					}
					(*pointers::chat_box)->iHideChatWindowCount = *pointers::sm_systemTime_m_Time;
				}
				HANDLER_END;
			});
		}
	};
}