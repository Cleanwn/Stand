#pragma once

#include "struct_base.hpp"

#include "CNetworkSession.hpp"
#include "rlTaskBase.hpp"
#include "rlGamerInfo.hpp"

#pragma pack(push, 1)
namespace rage
{
	template <class T>
	class rlTask : public rlTaskBase
	{
	};

	class snTask : public rlTask<snSession>
	{
		INIT_PAD(rlTask<snSession>, 0xA0) snSession* session;
	};
	static_assert(sizeof(snTask) == 0xA0 + 8);

	struct snJoinSessionTask : public snTask
	{
		INIT_PAD(snTask, 0xE0) uint32_t state;
		PAD(0xE4, 0xE8) rlGamerInfo local_player;
		PAD(0xE8 + 0x96, 0x180) rlSessionInfo session_info;
		/* 0x1F0 */ rlNetworkMode m_NetMode;
		PAD(0x1F4, 0x1F8) rlSlotType m_SlotType;
		PAD(0x1F8 + 4, 0x9B0) rlPeerInfo host;
	};
	//static_assert(sizeof(snJoinSessionTask) == 0x9B0 + sizeof(rlPeerInfo));

	struct snMigrateSessionTask : public snTask
	{
		INIT_PAD(snTask, 0xF0) rlPeerInfo peers[32];
		PAD(0xF0 + (0x68 * 32), 0xE88) rlSessionInfo updated_session_info;
	};
	//static_assert(sizeof(snMigrateSessionTask) == 0xE88 + sizeof(rlSessionInfo));
}
#pragma pack(pop)
