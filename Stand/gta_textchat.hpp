#pragma once

#include <string>
#include <vector>

#include "gta_fwddecl.hpp"

#include "atMap.hpp"
#include "rlGamerInfo.hpp"
#include "sysCriticalSection.hpp"

namespace rage
{
	struct TextChat
	{
		struct Typer;
		struct Peer;

		struct Typer
		{
			/* 0x000 */ rlGamerInfo m_GamerInfo;
			/* 0x0F8 */ Peer* m_Peer;
			/* 0x100 */ Typer* m_Next;
			/* 0x108 */ int32_t m_IsTypingCountdown;
			/* 0x10C */ bool m_IsActive : 1;
		};
		//static_assert(sizeof(Typer) == 0x110);
		//static_assert(offsetof(Typer, m_IsTypingCountdown) == 0x108);

		struct Peer
		{
			/* 0x00 */ int32_t m_ConnectionId;
			/* 0x04 */ uint32_t m_EndpointId;
			/* 0x08 */ Typer* m_Typers[1];
			/* 0x10 */ uint32_t m_NumTypers;
		};
		static_assert(offsetof(Peer, m_NumTypers) == 0x10);

		PAD(0, 0x80) char m_TextToSend[256];
		/* 0x180 */ char unk_uuid[40];
		/* 0x180 */ int32_t m_TextChatRecipients; // -1 = Team, -2 = Everyone
		/* 0x188 */ Typer m_TyperPile[32];
		Typer* m_TyperPool;
		Typer* m_Typers[32];
		uint32_t m_NumTypers;
		PAD(0x18E0 + 4, 0x18E8) Typer* m_LocalTypers[1];
		PAD(0x18E8 + 8, 0x1EF0) Peer* m_PeerPool;
		Peer* m_Peers[32];
		uint32_t m_NumPeers;
		uint32_t m_MaxLocalTypers;
		uint32_t m_MaxRemoteTypers;
		uint32_t m_NumLocalTypers;
		netConnectionManager* m_CxnMgr;
		uint32_t m_ChannelId;
		uint32_t m_TransitionChanelId;
		/* 0x2BC8 */ uint32_t m_LastUpdateTime;
		PAD(0x2BC8 + 4, 0x2BD0) sysCriticalSectionToken m_CsTypers;
		/* 2BF8 */ bool m_InTransition : 1;
		/* 2BF8 */ bool m_Initialized : 1;
	};
	static_assert(offsetof(TextChat, m_TextToSend) == 0x80); // 1.69
	static_assert(offsetof(TextChat, unk_uuid) == 0x180); // 1.69
}

namespace Stand
{
	struct ChatHistoryMessage
	{
		int64_t sender_rid;
		std::string text;
		uint64_t timestamp;
	};
}

struct CNetworkTextChat
{
	struct ChatHistoryLine
	{
		char messsage[256];
		uint64_t timestamp; // rlGetPosixTime, "time since 1970"
	};
	static_assert(sizeof(ChatHistoryLine) == 0x108);

	struct PlayerChatHistory // The history + index seem to be part of a new inline struct now
	{
		/* 0x00 */ rage::rlGamerHandle gamerHandle;
		/* 0x10 */ ChatHistoryLine history[25];
		/* 0x19D8 */ int index; // next message will be inserted at this index
		PAD(0x19D8 + 4, 0x3C00); // NetworkTextFilter

		void compileHistory(std::vector<Stand::ChatHistoryMessage>& messages) const;
	};

	PAD(0, 0x70) CNetGamePlayer* m_myPlayer;
	/* 0x78 */ rage::TextChat m_TextChat;
	/* 0x2C78 */ bool m_TeamOnlyChat : 1; // 0x2B78 as of 2944. likely rage::TextChat shrinked.
	/* 0x2C78 */ bool m_IsInitialized : 1;
	/* 0x2C78 */ bool m_IsSpectatorToNonSpectatorChatEnabled : 1;
	/* 0x2C78 */ bool m_IsTransitioning : 1;
	/* 0x2C78 */ bool m_IsScriptControllingTeams : 1;
	uint32_t m_AllowedSentFromGroups;
	/* 0x2C80 */ rage::atMap<uint64_t, PlayerChatHistory> m_ChatHistory;

	using ChatHistoryEntry = rage::atMapEntry<uint64_t, PlayerChatHistory>;
	static_assert(offsetof(ChatHistoryEntry, next) == 0x3C08);

	// Does not list all messages, e.g. messages including colons.
	[[nodiscard]] std::vector<Stand::ChatHistoryMessage> compileHistory() const;
};
//static_assert(offsetof(CNetworkTextChat, m_ChatHistory) == 0x2C80);
