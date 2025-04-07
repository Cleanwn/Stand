#pragma once

#include "CommandToggle.hpp"

#include "Chat.hpp"
#include "CMultiplayerChat.hpp"
#include "gta_input.hpp"
#include "is_session.hpp"

namespace Stand
{
	class CommandOpenChatWhenScrolling : public CommandToggle
	{
	public:
		explicit CommandOpenChatWhenScrolling(CommandList* const parent)
			: CommandToggle(parent, LOC("OCWS"), {}, LOC("OCWS_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler([this]
			{
				HANDLER_CHECK(this->m_on);

				if (is_session_started()
					&& !Chat::isOpen()
					)
				{
					if (PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_SCRIPTED_FLY_ZUP)
						|| PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_SCRIPTED_FLY_ZDOWN)
						)
					{
						pointers::CMultiplayerChat_SetFocus(*pointers::chat_box, 1, 0);
						(*pointers::chat_box)->iHideChatWindowCount = *pointers::sm_systemTime_m_Time;
					}
				}

				HANDLER_END;
			});
		}
	};
}