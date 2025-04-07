#pragma once

#include <cstdint>

#include "datBase.hpp"
#include "rlGamerHandle.hpp"

#pragma pack(push, 1)
namespace rage
{
	class netGamePresenceEvent
	{
	public:
		virtual ~netGamePresenceEvent() = default;
		virtual void _0x08() {};
		virtual const char* GetName() { return nullptr; };
		virtual bool _0x18() { return false; };
		// the following are not actually part of this class, maybe pure virtual
		virtual void Parse(void*) {};
		virtual void Process() {};
	};
	static_assert(sizeof(netGamePresenceEvent) == 8);
}

class CGamePresenceEventDispatcher : public rage::datBase
{
};

class CFingerOfGodPresenceEvent : public rage::netGamePresenceEvent
{
public:
	enum eFingerOfGodEvents : uint32_t
	{
		KICK = 0x84643284,
		KICK_BLACKLIST = 0x1D106C54,
		KILL_PED = 0xC2F12A5B,
		RAGDOLL_PED = 0x4349E22B,
		SPAWN_VEHICLE = 0xDD245B9C,
	};

	uint32_t type;
};
static_assert(sizeof(CFingerOfGodPresenceEvent) == 8 + 4);

class CGameInviteReply : public rage::netGamePresenceEvent
{
	rage::rlGamerHandle gamer_handle;
};
static_assert(sizeof(CGameInviteReply) == 8 + sizeof(rage::rlGamerHandle));
#pragma pack(pop)
