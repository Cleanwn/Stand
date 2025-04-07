#pragma once

#include "gta_player.hpp"
#include "gta_node_list.hpp"
#include "script_id.hpp"

#pragma pack(push, 1)
namespace rage
{
	struct netEventID
	{
		/* 0x00 */ uint32_t id;
		/* 0x04 */ playerbitset_t player_flags;
	};

	class netGameEvent
	{
	public:
		virtual ~netGameEvent() = default;

		virtual const char* GetEventName() { return 0; };
		virtual bool IsInScope(netPlayer* player) { return 0; };
		virtual bool TimeToResend(unsigned int) { return 0; };
		virtual bool CanChangeScope() { return 0; };

		virtual void Prepare(datBitBuffer* buffer, netPlayer* source_player, netPlayer* target_player) {};
		virtual void Handle(datBitBuffer* buffer, netPlayer* source_player, netPlayer* target_player) {};

		virtual bool Decide(const netPlayer& source_player, const netPlayer& target_player) { return 0; };

		virtual void PrepareReply(datBitBuffer* buffer, netPlayer* reply_player) {};
		virtual void HandleReply(datBitBuffer* buffer, netPlayer* souce_player) {};

		virtual void PrepareExtraData(datBitBuffer* buffer, bool is_reply, netPlayer* player, netPlayer* player2) {};
		virtual void HandleExtraData(datBitBuffer* buffer, bool is_reply, netPlayer* player, netPlayer* player2) {};

		virtual void WriteEventToLogFile() {};
		virtual void WriteDecisionToLogFile() {};
		virtual void WriteReplyToLogFile() {};
		virtual void WriteExtraDataToLogFile() {};

		virtual bool operator==(netGameEvent const& other) { return 0; };
		virtual bool operator!=(netGameEvent const& other) { return 0; };

		virtual bool MustPersist() { return 0; };
		virtual bool MustPersistWhenOutOfScope() { return 0; };
		virtual bool HasTimedOut() { return 0; };

		void FlagForRemoval() noexcept
		{
			flags |= 4;
		}

		[[nodiscard]] bool IsFlaggedForRemoval() const noexcept
		{
			return (flags & 4) != 0;
		}

		/* 0x08 */ uint16_t type;
		/* 0x0A */ bool requires_reply;
		PAD(0x0B, 0x10) netEventID* event_id;
		/* 0x18 */ netPlayer* owner_player;
		/* 0x20 */ uint32_t time_last_sent;
		PAD(0x24, 0x26) uint8_t flags;
		PAD(0x27, 0x30);
	};
	static_assert(sizeof(netGameEvent) == 0x30);

	class netEventMgr
	{
	public:
		PAD(0x00, 0x28) atDList<netGameEvent*> m_EventList; // Pending replies/acks from other players or pending to be sent.

		[[nodiscard]] netGameEvent* getLatestEvent() const
		{
			SOUP_IF_LIKELY (auto tail = m_EventList.Tail)
			{
				return tail->Data;
			}

			return nullptr;
		}
	};
}

class CScriptedGameEvent : public rage::netGameEvent
{
public:
	char m_padding[0x40];      // 0x30
	int64_t m_args[54];   // 0x70
	uint32_t m_bitset;    // 0x220
	uint32_t m_args_size; // 0x224
};

class CNetworkIncrementStatEvent : public rage::netGameEvent
{
public:
	int32_t m_stat;    // 0x30
	uint32_t m_amount; // 0x34
};

class CPlaySoundEvent : public rage::netGameEvent
{
public:
	/* 0x30 */ bool m_bUseEntity;
	PAD(0x30 + 1, 0x32) rage::ObjectId m_EntityId;
	PAD(0x32 + 2, 0x40) rage::Vector3 m_Position;
	/* 0x50 */ uint32_t m_setNameHash;
	/* 0x54 */ uint32_t m_soundNameHash;
	/* 0x58 */ uint8_t m_SoundID;
	PAD(0x58 + 1, 0x60) CGameScriptId m_ScriptId;
	/* 0xA0 */ int m_nRange;
};
static_assert(offsetof(CPlaySoundEvent, m_nRange) == 0xA0);

class CStopSoundEvent : public rage::netGameEvent
{
public:
	/* 0x30 */ uint8_t sound_id;
};
static_assert(sizeof(CStopSoundEvent) == 0x30 + 1);

class CPlayerCardStatEvent : public rage::netGameEvent
{
public:
	/* 0x30 */ float stats[15];
};
static_assert(sizeof(CPlayerCardStatEvent) == 0x6C);
#pragma pack(pop)
