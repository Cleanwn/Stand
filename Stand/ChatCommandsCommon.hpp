#pragma once

#include <string>

#include "fwddecl.hpp"

namespace Stand
{
	struct ChatCommandsCommon
	{
		static inline CommandListSelect* prefix = {};
		static inline CommandListSelect* reply_prefix = {};
		static inline CommandListSelect* reply_visibility = {};

		static const wchar_t getEffectiveChatPrefix();
		static const char* getEffectiveReplyPrefix();

		static void processMessageFromSenderWithChatCommandPermissions(evtChatEvent& e);
	};
}
