#pragma once

#include "rl.hpp"
#include "rlPeerInfo.hpp"
#include "rlSessionInfo.hpp"
#include "rlSessionConfig.hpp"
#include "rlGamerHandle.hpp"

namespace rage
{
	struct rlSessionDetail
	{
		/* 0x00 */ rlPeerInfo m_HostPeerInfo;
		/* 0x68 */ rlSessionInfo m_SessionInfo;
		/* 0xD8 */ rlSessionConfig m_SessionConfig;
		rlGamerHandle m_HostHandle;
		char m_HostName[RL_MAX_NAME_BUF_SIZE];
		unsigned m_NumFilledPublicSlots;
		unsigned m_NumFilledPrivateSlots;
		uint16_t m_SessionUserDataSize;
		uint8_t m_SessionUserData[RL_MAX_SESSION_USER_DATA_SIZE];
		uint16_t m_SessionInfoMineDataSize;
		uint8_t m_SessionInfoMineData[RL_MAX_SESSION_DATA_MINE_SIZE];
	};
	//static_assert(offsetof(rlSessionDetail, m_SessionConfig) == 0xD8);

	struct rlSessionDetailResponse
	{
		uint32_t m_Response;
		uint32_t m_ResponseParam;
		uint32_t m_UniqueID;
		rlPeerInfo m_HostPeerInfo;
		rlSessionInfo m_SessionInfo;
		rlSessionConfig m_Config;
		rlGamerHandle m_HostHandle;
		char m_HostName[RL_MAX_NAME_BUF_SIZE];
		uint32_t m_NumFilledPublicSlots;
		uint32_t m_NumFilledPrivateSlots;
	};
}
