#pragma once

#include "netMessage.hpp"
#include "netMessageId.hpp"

#include "gta_player.hpp"
#include "netAddress.hpp"
#include "rl.hpp"
#include "rlGamerInfo.hpp"

#define SNET_MAX_SIZEOF_GAMER_DATA 512
#define SNET_MAX_SIZEOF_JOIN_RESPONSE_DATA 512

namespace rage
{
	struct snMsgBase : public netMessage
	{
		uint64_t session_id;

	protected:
		template <typename T>
		bool ser(T& bb)
		{
			return bb.serU64(session_id);
		}
	};
	static_assert(sizeof(snMsgBase) == 8);

	// This packet is sent either by the host to remove a gamer,
	// or by the gamer themselves to notify of their departure (although not always).
	struct snMsgRemoveGamersFromSessionCmd : public snMsgBase
	{
		AUTOID_MSG(SN_MSG_REMOVE_GAMERS_FROM_SESSION_CMD);

		/* 0x008 */ rlGamerHandle targets[32];
		/* 0x208 */ int32_t remove_reason; // eBailReason
		/* 0x20C */ uint32_t num_targets;

		template <typename T>
		bool ser(T& bb)
		{
			if (snMsgBase::ser(bb)
				&& bb.serU32(num_targets, 6)
				&& num_targets <= MAX_PLAYERS
				)
			{
				for (uint32_t i = 0; i != num_targets; ++i)
				{
					if (!targets[i].ser(bb))
					{
						return false;
					}
				}
				return bb.serI32(remove_reason, 16);
			}
			return false;
		}
	};
	static_assert(sizeof(snMsgRemoveGamersFromSessionCmd) == 0x20C + 4);

	struct snMsgAddGamerToSessionBase : public snMsgBase
	{
		/* 0x08 */ rlGamerInfo m_GamerInfo;
		/* 0xF8 */ netAddress m_RelayAddr;
		/* 0x118 */ rlSlotType m_SlotType;
		/* 0x11C */ unsigned int m_SizeofData;
		/* 0x120 */ uint8_t m_Data[SNET_MAX_SIZEOF_GAMER_DATA];

		/*template <typename T>
		bool ser(T& bb)
		{
			return snMsgBase::ser(bb)
				&& m_GamerInfo.ser(bb)
				&& m_RelayAddr.ser(bb)
				&& bb.serU32(m_SlotType, 2)
				&& bb.serU32(m_SizeofData, 10)
				&& m_SizeofData <= SNET_MAX_SIZEOF_GAMER_DATA
				&& bb.serBytes(m_Data, m_SizeofData)
				;
		}*/
	};
	static_assert(sizeof(snMsgAddGamerToSessionBase) == 0x120 + SNET_MAX_SIZEOF_GAMER_DATA);
	static_assert(offsetof(snMsgAddGamerToSessionBase, m_RelayAddr) == 0xF8);
	static_assert(offsetof(snMsgAddGamerToSessionBase, m_SlotType) == 0x118);
	static_assert(offsetof(snMsgAddGamerToSessionBase, m_SizeofData) == 0x11C);
	static_assert(offsetof(snMsgAddGamerToSessionBase, m_Data) == 0x120);

	struct snMsgAddGamerToSessionCmd : public snMsgAddGamerToSessionBase
	{
		unsigned m_ReservedSize;
		rlGamerHandle m_ReservedMembers[RL_MAX_GAMERS_PER_SESSION];
	};

	struct snMsgJoinRequest : public snMsgAddGamerToSessionBase
	{
		AUTOID_MSG(SN_MSG_JOIN_REQUEST);

		/* 0x320 */ uint32_t m_GroupSize;
		/* 0x328 */ rlGamerHandle m_GroupMembers[RL_MAX_GAMERS_PER_SESSION];

		/*template <typename T>
		bool ser(T& bb)
		{
			if (snMsgAddGamerToSessionBase::ser(bb)
				&& bb.serU32(m_GroupSize, 6)
				&& m_GroupSize <= MAX_PLAYERS
				)
			{
				for (uint8_t i = 0; i != m_GroupSize; ++i)
				{
					if (!m_GroupMembers[i].ser(bb))
					{
						return false;
					}
				}
				return true;
			}
			return false;
		}*/
	};
	static_assert(offsetof(snMsgJoinRequest, m_GroupSize) == 0x320);
	static_assert(offsetof(snMsgJoinRequest, m_GroupMembers) == 0x328);
}

struct MsgKickPlayer : public rage::netMessage
{
	AUTOID_MSG(MSG_KICK_PLAYER);

	/* 0x00 */ uint32_t type;
	PAD(0x04, 0x08) uint32_t unk;
	PAD(0x0C, 0x18) bool unk_bool;

	template <typename T>
	bool ser(T& bb)
	{
		return bb.serU32(type, 5)
			&& bb.serU32(unk)
			&& bb.serBool(unk_bool)
			;
	}
};
