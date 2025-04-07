#pragma once

#include <cstdint>

namespace rage
{
	enum snEventId : uint64_t
	{
		SESSION_EVENT_HOSTED = 0, // snEventSessionHosted
		SESSION_EVENT_JOINED = 1, // snEventSessionJoined
		SESSION_EVENT_JOIN_FAILED = 3, // snEventJoinFailed
		SESSION_EVENT_ADDED_GAMER = 6, // snEventAddedGamer
		SESSION_EVENT_REMOVED_GAMER = 7, // snEventRemovedGamer
		SESSION_EVENT_ESTABLISHED = 8, // snEventSessionEstablished
		SESSION_EVENT_MIGRATE_START = 9, // snEventSessionMigrateStart
		SESSION_EVENT_MIGRATE_END = 10, // snEventSessionMigrateEnd
		SESSION_EVENT_ATTRS_CHANGED = 11, // snEventSessionAttrsChanged
		SESSION_EVENT_DESTROYED = 12, // snEventSessionDestroyed
		SESSION_EVENT_REMOVED_DUE_TO_COMPLAINTS = 14,
		SESSION_EVENT_REMOVED_DUE_TO_STALL = 15,
	};

	// rage::rlSessionEvent in newer rage versions
	class snEvent
	{
	public:
		virtual snEventId GetId() const = 0;
		virtual ~snEvent() = default;

		/* 0x08 */ snEvent* unk_thisptr;
		PAD(0x10, 0x20)
	};
	static_assert(sizeof(snEvent) == 0x20);

	class snEventAddedGamer : public snEvent
	{
	public:
		/* 0x20 */ rlGamerInfo gamer;
	};

	// snet/session.h
	class snEventRemovedGamer : public snEvent
	{
	public:
		/* 0x20 */ rlGamerInfo gamer;
		PAD(0x110, 0x118) int remove_reason;
	};
	static_assert(offsetof(snEventRemovedGamer, remove_reason) == 0x118);
}

using enum rage::snEventId;
