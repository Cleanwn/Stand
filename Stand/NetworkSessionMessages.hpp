#pragma once

#include "netMessage.hpp"
#include "netMessageId.hpp"

#include "rl.hpp"
#include "rlGamerHandle.hpp"
#include "rlSessionInfo.hpp"

struct MsgBlacklist : public rage::netMessage
{
	AUTOID_MSG(MSG_BLACKLIST);

	uint64_t session_id;
	rage::rlGamerHandle gamer_handle;
	int reason;

	template <typename T>
	bool ser(T& bb)
	{
		return bb.serU64(session_id)
			&& gamer_handle.ser(bb)
			&& bb.serI32(reason, 2)
			;
	}
};

// Changed in 2845.
struct MsgLostConnectionToHost : public rage::netMessage
{
	AUTOID_MSG(MSG_LOST_HOST_CONNECTION);

	template <typename T>
	bool ser(T& bb)
	{
		return true;
	}
};

struct MsgRadioStationSyncRequest : public rage::netMessage
{
	AUTOID_MSG(MSG_RADIO_STATION_SYNC_REQUEST);

	template <typename T>
	bool ser(T& bb)
	{
		return true;
	}
};

// This doesn't seem to be in the game anymore.
/*struct MsgSessionEstablishedRequest : public rage::netMessage
{
	AUTOID_MSG(MSG_SESSION_ESTABLISHED_REQUEST);

	uint64_t session_id;

	template <typename T>
	bool ser(T& bb)
	{
		return bb.serU64(session_id);
	}
};*/

struct MsgTransitionGamerInstruction : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_GAMER_INSTRUCTION);

	rage::rlGamerHandle m_hFromGamer;
	rage::rlGamerHandle m_hToGamer;
	char m_szGamerName[RL_MAX_NAME_BUF_SIZE];
	int m_nInstruction;
	int m_nInstructionParam;
	unsigned m_uUniqueToken;

	template <typename T>
	bool ser(T& bb)
	{
		return m_hFromGamer.ser(bb)
			&& m_hToGamer.ser(bb)
			&& bb.serStr(m_szGamerName, RL_MAX_NAME_BUF_SIZE)
			&& bb.serI32(m_nInstruction)
			&& bb.serI32(m_nInstructionParam)
			&& bb.serU32(m_uUniqueToken)
			;
	}
};

struct MsgTransitionLaunch : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_LAUNCH);

	uint32_t m_HostTimeToken;
	uint32_t m_HostTime;
	uint32_t m_ModelVariationID;
	uint64_t m_TimeCreated;

	template <typename T>
	bool ser(T& bb)
	{
		return bb.serU32(m_HostTimeToken)
			&& bb.serU32(m_HostTime)
			&& bb.serU32(m_ModelVariationID, 3)
			&& bb.serU64(m_TimeCreated)
			;
	}
};

struct MsgTransitionLaunchNotify : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_LAUNCH_NOTIFY);

	//rage::rlGamerHandle gamer_handle; // was removed at some point

	template <typename T>
	bool ser(T& bb)
	{
		return true;
	}
};

struct MsgTransitionToGameStart : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_TO_GAME_START);

	rage::rlGamerHandle m_hGamer;
	int32_t m_GameMode;
	bool m_IsTransitionFromLobby;

	template <typename T>
	bool ser(T& bb)
	{
		return m_hGamer.ser(bb)
			&& bb.serI32(m_GameMode, 16)
			&& bb.serBool(m_IsTransitionFromLobby)
			;
	}
};

struct MsgTransitionToGameNotify : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_TO_GAME_NOTIFY);

	enum eNotificationType
	{
		NOTIFY_SWAP = 0,
		NOTIFY_UPDATE = 1,
	};

	rage::rlGamerHandle m_hGamer;
	rage::rlSessionInfo m_SessionInfo;
	uint8_t m_NotificationType;
	bool m_bIsPrivate;

	/*template <typename T>
	bool ser(T& bb)
	{
		return m_hGamer.ser(bb)
			&& m_SessionInfo.ser(bb)
			&& bb.serU8(m_NotificationType, 2)
			&& bb.serBool(m_bIsPrivate)
			;
	}*/
};

struct MsgTransitionToActivityStart : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_TO_ACTIVITY_START);

	rage::rlGamerHandle m_hGamer;
	bool m_bIsAsync;

	template <typename T>
	bool ser(T& bb)
	{
		return m_hGamer.ser(bb)
			&& bb.serBool(m_bIsAsync)
			;
	}
};

struct MsgTransitionToActivityFinish : public rage::netMessage
{
	AUTOID_MSG(MSG_TRANSITION_TO_ACTIVITY_FINISH);

	bool m_bDidSucceed;
	rage::rlGamerHandle m_hGamer;
	rage::rlSessionInfo m_SessionInfo;
	bool m_bIsPrivate;

	/*template <typename T>
	bool ser(T& bb)
	{
		return bb.serBool(m_bDidSucceed)
			&& m_hGamer.ser(bb)
			&& (m_bDidSucceed ? m_SessionInfo.ser(bb) : true)
			&& (m_bDidSucceed ? bb.serBool(m_bIsPrivate) : true)
			;
	}*/
};

struct MsgCheckQueuedJoinRequestReply : public rage::netMessage
{
	AUTOID_MSG(MSG_CHECK_QUEUED_JOIN_REQUEST_REPLY);

	uint32_t m_nUniqueToken;
	bool m_bReservedSlot;
	uint64_t m_nSessionToken;

	template <typename T>
	bool ser(T& bb)
	{
		return bb.serU32(m_nUniqueToken)
			&& bb.serBool(m_bReservedSlot)
			&& bb.serU64(m_nSessionToken)
			;
	}
};
