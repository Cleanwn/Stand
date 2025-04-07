#pragma once

#pragma pack(push, 1)
namespace rage
{
	// within ctor: C7 83 ? ? 00 00 01 00 00 00 44 89 B3
	class VoiceChat
	{
	public:
		struct Talker
		{
			rlGamerInfo m_GamerInfo;
			PAD(0xF0, 0x118);
		};
		static_assert(sizeof(Talker) == 0x118); // 3028

		/* 0x0000 */ Talker m_TalkerPile[32];
		/* 0x2300 */ Talker* m_TalkerPool;
		rlGamerInfo* m_Talkers[32];
		uint32_t m_NumTalkers;
	};
	static_assert(offsetof(VoiceChat, m_TalkerPool) == 0x2300); // 3028
}

class CNetworkVoice
{
	PAD(0, 0x30) rage::VoiceChat m_VoiceChat;
	PAD(0x30 + sizeof(rage::VoiceChat), 0x36EC) uint32_t m_TextAllowedSentFromGroups;
	/* 0x36F0 */ bool m_UsePresenceForTextMessages;
	/* 0x36F1 */ bool m_DisableReceivingTextMessages;
	/* 0x36F2 */ bool m_IsTextFilterEnabled;
};
static_assert(offsetof(CNetworkVoice, m_VoiceChat) == 0x30);
static_assert(offsetof(CNetworkVoice, m_TextAllowedSentFromGroups) == 0x36EC);
static_assert(offsetof(CNetworkVoice, m_UsePresenceForTextMessages) == 0x36F0);
static_assert(offsetof(CNetworkVoice, m_DisableReceivingTextMessages) == 0x36F1);
static_assert(offsetof(CNetworkVoice, m_IsTextFilterEnabled) == 0x36F2);
#pragma pack(pop)
