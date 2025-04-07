#pragma once

#include "struct_base.hpp"

#include "rlGamerInfo.hpp"

#pragma pack(push, 1)
namespace rage
{
	enum rlPresenceEventId : uint64_t
	{
		JOINED_VIA_PRESENCE = 0x7,
	};

	struct rlPresenceEvent
	{
		virtual rlPresenceEventId GetId() const { return (rlPresenceEventId)0; };
		virtual ~rlPresenceEvent() = default;

		rlPresenceEvent(rage::rlGamerInfo& gamer_info)
			: gamer_info(gamer_info)
		{
			thisptr = this;
		}

		/* 0x08 */ rlPresenceEvent* thisptr;
		/* 0x10 */ rlGamerInfo gamer_info{};
	};
	static_assert(sizeof(rlPresenceEvent) == 0x10 + sizeof(rlGamerInfo));

	struct rlPresenceEventJoinedViaPresence : public rlPresenceEvent
	{
		rlPresenceEventId GetId() const final { return JOINED_VIA_PRESENCE; };

		PAD(0x106, 0x108) rlSessionInfo session_info{};
		/* 0x1D8 */ rlGamerHandle gamer_handle{};
		/* 0x1E8 */ int source = 0; // 0 = steamjvp, 1 = ros.mp.invite

		rlPresenceEventJoinedViaPresence(rage::rlGamerInfo& gamer_info, rage::rlSessionInfo& session_info)
			: rlPresenceEvent(gamer_info), session_info(session_info)
		{
		}
	};
	static_assert(sizeof(rlPresenceEventJoinedViaPresence) == 0x1E8 + sizeof(int));
}
#pragma pack(pop)
