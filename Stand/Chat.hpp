#pragma once

#include <string>

#include "fwddecl.hpp"

#include "RateLimit.hpp"
#include "Spinlock.hpp"

namespace Stand
{
	class Chat
	{
	public:
		inline static Spinlock mtx{};
		inline static RateLimit packet_ratelimit{ 3000, 600 };

	public:
		[[nodiscard]] static bool isOpen();
		static void close(); // needs yieldable script thread and ownership of chat mutex
		static void openImpl(bool team_chat); // needs ownership of chat mutex
		static void open(bool team_chat); // needs yieldable script thread and ownership of chat mutex
		static void sendMessage(const std::string& text, bool team_chat, bool add_to_local_history, bool networked, bool exclude_ad = false, bool trigger_event = true, bool already_censored = false);
		static void sendTargetedMessageWithReplacements(AbstractPlayer recipient, std::string text, bool team_chat, bool exclude_ad = false, bool already_censored = false);
		static void sendTargetedMessage(AbstractPlayer recipient, const std::string& text, bool team_chat, bool exclude_ad = false, bool already_censored = false);
		static void sendTargetedMessage(AbstractPlayer recipient, AbstractPlayer sender, const std::string& text, bool team_chat, bool exclude_ad = false, bool already_censored = false);
	private:
		static void ensureOpen(bool team_chat); // needs yieldable script thread and ownership of chat mutex
		static void ensureOpenWithEmptyDraftImpl(bool team_chat); // needs yieldable script thread and ownership of chat mutex
	public:
		static void ensureOpenWithEmptyDraft(bool team_chat); // needs yieldable script thread
		static void addToDraft(const std::wstring& message);
	private:
		static void removeFromDraftImpl(uint16_t i);
	public:
		static void removeFromDraft(uint16_t i);

		[[nodiscard]] static int getNumRecipientsExcludingSelf(bool team_chat);

		// As of 3028, messages with ideographic space no longer reach the scaleform, making this useless.

		static void markAsAuto(std::string& msg)
		{
			//msg.append((const char*)u8"　");
		}

		[[nodiscard]] static bool isMarkedAsAuto(const std::string& msg)
		{
			return false;
			//return msg.length() > 3 && msg.substr(msg.length() - 3) == (const char*)u8"　";
		}

		static void removeAutoMark(std::string& msg)
		{
			//msg.erase(msg.length() - 3);
		}
	};
}
