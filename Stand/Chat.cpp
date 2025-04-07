#include "Chat.hpp"

#include <soup/Uuid.hpp>

#include "CMultiplayerChat.hpp"
#include "evtChatEvent.hpp"
#include "CMsgTextMessage.hpp"
#include "CNetGamePlayer.hpp"
#include "gta_input.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "SpinlockHolder.hpp"
#include "StringUtils.hpp"

#define ENSURE_ONLINE(...) MULTI_MACRO(ENSURE_ONLINE, __VA_ARGS__)
#define ENSURE_ONLINE_0() ENSURE_ONLINE_1(;)
#define ENSURE_ONLINE_1(default_ret) \
if (!is_session_or_transition_active(false)) \
{ \
	return default_ret; \
}

#define ENSURE_ONLINE_AND_LOCK_CHAT_MTX \
ENSURE_ONLINE(); \
SpinlockHolder lock(mtx); \
while (!lock.tryLock()) \
{ \
	Script::current()->yield(); \
}

namespace Stand
{
	inline static float open_alt = 1.0f;

	bool Chat::isOpen()
	{
		ENSURE_ONLINE(false);
		return (*pointers::chat_box)->is_open && (*pointers::chat_box)->state != CMultiplayerChat::CLOSED;
	}

	void Chat::close()
	{
		//ENSURE_ONLINE_AND_LOCK_CHAT_MTX
		while (isOpen())
		{
			HUD::CLOSE_MP_TEXT_CHAT();
			Script::current()->yield();
		}
	}

	void Chat::openImpl(bool team_chat)
	{
		PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, team_chat ? INPUT_MP_TEXT_CHAT_TEAM : INPUT_MP_TEXT_CHAT_ALL, open_alt);
		open_alt = open_alt == 1.0f ? 0.0f : 1.0f;
	}

	void Chat::open(bool team_chat)
	{
		//ENSURE_ONLINE_AND_LOCK_CHAT_MTX
		do
		{
			openImpl(team_chat);
			Script::current()->yield();
		} while (!isOpen());
	}

	void Chat::sendMessage(const std::string& text, bool team_chat, bool add_to_local_history, bool networked, bool exclude_ad, bool trigger_event, bool already_censored)
	{
		if (trigger_event)
		{
			evtChatEvent::trigger(evtChatEvent(
				g_player,
				text,
				team_chat,
				networked,
				true
			));
		}

		if (add_to_local_history)
		{
			sendTargetedMessageWithReplacements(g_player, text, team_chat, false, already_censored);
		}

		if (networked)
		{
			if (AbstractPlayer boss; !team_chat || (boss = g_player.getBoss()).isValid())
			{
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					if (!team_chat || p.getBoss() == boss)
					{
						sendTargetedMessageWithReplacements(p, text, team_chat, exclude_ad, already_censored);
					}
				}
			}
		}
	}

	void Chat::sendTargetedMessageWithReplacements(AbstractPlayer recipient, std::string text, bool team_chat, bool exclude_ad, bool already_censored)
	{
		if (text.substr(0, 5) == "TEAM:")
		{
			text.erase(0, 5);
			team_chat = true;
		}
		StringUtils::replace_all(text, "${nl}", "\n");
		StringUtils::replace_all(text, "${name}", recipient.getName());
		auto ip = recipient.getAddress().ip;
		StringUtils::replace_all(text, "${ip}", ip.toString<std::string>());
		StringUtils::replace_all(text, "${geoip.isp}", ip.getGeoIpField(GEOIP_ISP));
		StringUtils::replace_all(text, "${geoip.country}", ip.getGeoIpField(GEOIP_COUNTRY));
		StringUtils::replace_all(text, "${geoip.region}", ip.getGeoIpField(GEOIP_STATE));
		StringUtils::replace_all(text, "${geoip.city}", ip.getGeoIpField(GEOIP_CITY));

		if (g_hooking.shouldBypassProfanityFilter(team_chat))
		{
			text = g_hooking.makeUncensoredChatMessage(text);
		}

		while (text.size() > 255)
		{
			sendTargetedMessage(recipient, text.substr(0, 255), team_chat, exclude_ad, already_censored);
			text.erase(0, 255);
		}
		sendTargetedMessage(recipient, text, team_chat, exclude_ad, already_censored);
	}

	void Chat::sendTargetedMessage(AbstractPlayer recipient, const std::string& text, bool team_chat, bool exclude_ad, bool already_censored)
	{
		sendTargetedMessage(recipient, g_player, text, team_chat, exclude_ad, already_censored);
	}

	void Chat::sendTargetedMessage(AbstractPlayer recipient, AbstractPlayer sender, const std::string& text, bool team_chat, bool exclude_ad, bool already_censored)
	{
		if (recipient == g_player)
		{
			g_hooking.can_modify_chat_labels = true;
			pointers::CMultiplayerChat_OnEvent(*pointers::chat_box, 0, sender.getGamerHandle(), text.c_str(), team_chat);
			g_hooking.can_modify_chat_labels = false;
		}
		else if (auto cngp = recipient.getCNetGamePlayer())
		{
			if (packet_ratelimit.canRequest())
			{
				packet_ratelimit.addRequest();

				rage::CMsgTextMessage packet;
				// No one will censor the message for us since we're proceeding to send the packet directly and recipients will not show an uncensored message.
				// However, If we do have bypass profanity filter on, "fuck" turns into "**c*" now because "fuk" is in the profanity list, so just not doing this if the message was already censored. Note that people still receive our homoglyph'd "fuck".
				if (already_censored)
				{
					packet.setText(text.c_str());
				}
				else
				{
					auto censored = g_hooking.censorChatMessage(text);
					packet.setText(censored.c_str());
				}
				packet.sender = g_player.getGamerHandle(); // force sender to be g_player
				{
					auto uuid = soup::Uuid::v4();
					// They use UUIDv1, so pretend we do, too.
					uuid.bytes[7] &= 0x0F;
					uuid.bytes[7] |= 0x10;
					auto uuid_str = uuid.toString();
					strcpy(packet.unk_uuid, uuid_str.c_str());
				}
				packet.team_chat = team_chat;

				g_hooking.exclude_ad = exclude_ad;
				hooks::send_CMsgTextMessage(*pointers::connection_mgr, cngp->channel_7_cxn_id, &packet, 1, 0);
			}
		}
	}

	void Chat::ensureOpen(bool team_chat)
	{
		if (isOpen())
		{
			if ((*pointers::chat_box)->state == (team_chat ? CMultiplayerChat::TEAM : CMultiplayerChat::ALL))
			{
				return;
			}
			close();
		}
		open(team_chat);
	}

	void Chat::ensureOpenWithEmptyDraftImpl(bool team_chat)
	{
		ensureOpen(team_chat);
		removeFromDraftImpl((uint8_t)(*pointers::chat_box)->message_length);
	}

	void Chat::ensureOpenWithEmptyDraft(bool team_chat)
	{
		ENSURE_ONLINE_AND_LOCK_CHAT_MTX;
		ensureOpenWithEmptyDraftImpl(team_chat);
	}

	void Chat::addToDraft(const std::wstring& message)
	{
		ENSURE_ONLINE();
		const bool prev_bypass_character_filter = g_hooking.bypass_character_filter;
		g_hooking.bypass_character_filter = true;
		if (pointers::min_character_value != nullptr)
		{
			*pointers::min_character_value = 0;
		}
		for (const wchar_t& c : message)
		{
			*pointers::rage_ioKeyboard_sm_TextBuffer = c;
			hooks::CMultiplayerChat_UpdateInputWhenTyping(*pointers::chat_box);
		}
		if (pointers::min_character_value != nullptr)
		{
			*pointers::min_character_value = 0x1F;
		}
		*pointers::rage_ioKeyboard_sm_TextBuffer = 0;
		g_hooking.bypass_character_filter = prev_bypass_character_filter;
	}

	void Chat::removeFromDraftImpl(uint16_t i)
	{
		g_hooking.emulate_chat_message_backspace_press = i;
		while (g_hooking.emulate_chat_message_backspace_press > 0)
		{
			(*pointers::chat_box)->last_delete_tick = 0;
			hooks::CMultiplayerChat_UpdateInputWhenTyping(*pointers::chat_box);
		}
	}

	void Chat::removeFromDraft(uint16_t i)
	{
		ENSURE_ONLINE();
		removeFromDraftImpl(i);
	}

	int Chat::getNumRecipientsExcludingSelf(bool team_chat)
	{
		if (team_chat)
		{
			int count = 0;
			if (AbstractPlayer boss = g_player.getBoss(); boss.isValid())
			{
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					if (p.getBoss() == boss)
					{
						++count;
					}
				}
			}
			return count;
		}
		return AbstractPlayer::getPlayerCount() - 1;
	}
}
