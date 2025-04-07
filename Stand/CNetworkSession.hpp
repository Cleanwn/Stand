#pragma once

#include "fwddecl.hpp"

#include "inlist.hpp"
#include "net.hpp"
#include "netConnectionManager.hpp"
#include "netStatus.hpp"
#include "NetworkSessionMessages.hpp"
#include "rlGamerInfo.hpp"
#include "rlSessionConfig.hpp"
#include "rlSessionInfo.hpp"
#include "sysObfuscated.hpp"

#pragma pack(push, 1)
namespace rage
{
	class rlSession
	{
	public:
		struct Data
		{
			//rlGamerInfo local_gamer;
			PAD(0, 0x100) rlSessionInfo info;
			rlSessionConfig config;
			rlScMatchmakingMatchId m_MatchId;
		};
		//static_assert(sizeof(Data) == 0x118 + sizeof(rlSessionConfig));

		struct Cmd
		{
			virtual ~Cmd() = default;
			virtual void Update() {};

			enum eCmdType : uint32_t
			{
				HOST = 0,
				ACTIVATE = 2,
				ESTABLISH = 3,
				DESTROY = 4,
				JOIN = 5,
				LEAVE = 6,
				MIGRATE = 8,
				MODIFY_PRESENCE_FLAGS = 9,
				SEND_INVITES = 10,
				UPDATE_SC_ADVERTISEMENT = 12,
			};

			/* 0x08 */ eCmdType type;
			PAD(0x08 + 4, 0x10) rlSession* session;
		};
		static_assert(sizeof(Cmd) == 0x18);

		struct CmdHost : public Cmd
		{
			INIT_PAD(Cmd, 0x38) rlGamerInfo gamer_info;
		};
		static_assert(sizeof(CmdHost) == 0x38 + sizeof(rlGamerInfo));

		struct CmdMigrate : public Cmd
		{
			/* 0x18 */ uint32_t* unk_state_ptr;
			PAD(0x18 + 8, 0x28) uint32_t state; // 3 = finished, just need to set host
			PAD(0x28 + 4, 0x98) rlSessionInfo updated_session_info;
		};
		static_assert(sizeof(CmdMigrate) == 0x98 + sizeof(rlSessionInfo));

		virtual ~rlSession() = default;

		/* 0x008 */ Data data;
		PAD(0x008 + sizeof(Data), 0x770);
		//PAD(0x008 + 0x118 + sizeof(rlSessionConfig), 0x598) Cmd* cmd;
		//PAD(0x598 + 8, 0x8B4) uint8_t flags; // (1 << 1) = can update cmd?

		[[nodiscard]] uint64_t GetId() const
		{
			return data.config.session_id;
		}
	};
	static_assert(sizeof(rlSession) == 0x770); // 2944
	static_assert(offsetof(rlSession, data.info) == 0x108); // 2944
#pragma pack(pop)

	class snGamer
	{
	public:
		rlGamerInfo gamer_info;
		PAD(sizeof(rlGamerInfo), 0x100) inlist_node<snGamer> m_ListLink;
		PAD(0x110, 0x118);
	};
	static_assert(offsetof(snGamer, m_ListLink) == 0x100); // 2944

	class snPeer
	{
	public:
		/* 0x00 */ int32_t cxn_id;
		/* 0x08 */ rlPeerInfo peer_info;
		/* 0xD0 */ EndpointId endpoint_id;
		/* 0xD8 */ snGamer* gamer;
		/* 0xE0 */ uint32_t m_NumGamers;
		/* 0xE8 */ inlist_node<snPeer> m_ListLink;
		/* 0xF8 */ bool m_IsLocal : 1;
		/* 0xF8 */ bool m_AllowMigrate : 1;
	};
	static_assert(sizeof(snPeer) == 0x100);
	static_assert(offsetof(snPeer, gamer) == 0xD8); // 2944
	static_assert(offsetof(snPeer, m_NumGamers) == 0xE0); // 2944

#define RLSC_PRESENCE_ATTR_NAME_MAX_SIZE 64

	class snSession
	{
		typedef inlist<snGamer, &snGamer::m_ListLink> GamerList;
		typedef inlist<snPeer, &snPeer::m_ListLink> PeerList;

		PAD(0, 0x48) netConnectionManager* m_CxnMgr;
		/* 0x50 */ netConnectionManager::Delegate m_CxnMgrDlgt;
		/* 0x80 */ rlSession rline_session;
		/* 0x7F0 */ snPeer m_LocalPeer;
		/* 0x8F0 */ netPeerId host_peer_id;
		PAD(0x8F8, 0x988) snGamer m_GamerPile[RL_MAX_GAMERS_PER_SESSION];
		GamerList m_GamerPool;
		snGamer* m_Gamers[RL_MAX_GAMERS_PER_SESSION];
		uint32_t m_NumGamers;
		snPeer m_PeerPile[RL_MAX_GAMERS_PER_SESSION];
		PeerList m_PeerPool;
		snPeer* peers[RL_MAX_GAMERS_PER_SESSION];
		uint32_t num_peers;
		PAD(0x4EC0 + 4, 0x5338) uint32_t channel_id;
		uint32_t unk_possibly_added_in_2944_if_not_padding_weirdness;
		/* 0x5448 */ int32_t m_LocalOwnerIndex;
		/* 0x544C */ char m_AttrSessionToken[RLSC_PRESENCE_ATTR_NAME_MAX_SIZE]; // "gstok"
		/* 0x548C */ char m_AttrSessionId[RLSC_PRESENCE_ATTR_NAME_MAX_SIZE]; // "gsid"
		/* 0x54CC */ char m_AttrSessionInfo[RLSC_PRESENCE_ATTR_NAME_MAX_SIZE]; // "gsinfo"
		/* 0x550C */ char m_AttrIsHost[RLSC_PRESENCE_ATTR_NAME_MAX_SIZE]; // "gshost"
		/* 0x554C */ char m_AttrIsJoinable[RLSC_PRESENCE_ATTR_NAME_MAX_SIZE - 8]; // "gsjoin"
		void* unused_memory;

		[[nodiscard]] uint64_t GetSessionId() const
		{
			return rline_session.GetId();
		}

		[[nodiscard]] snPeer* FindPeerByCxn(int32_t id) const;
		[[nodiscard]] snPeer* FindPeerById(netPeerId peerId) const;
	};
	// 2944
	static_assert(sizeof(snSession) == 0x5488); // see CNetworkSession ctor
	static_assert(offsetof(snSession, rline_session) == 0x80);
	static_assert(offsetof(snSession, m_LocalPeer) == 0x7F0);
	static_assert(offsetof(snSession, host_peer_id) == 0x8F0);
	static_assert(offsetof(snSession, m_GamerPile) == 0x988);
	static_assert(offsetof(snSession, m_GamerPool) == 0x2C88);
	static_assert(offsetof(snSession, peers) == 0x4DC0);
	static_assert(offsetof(snSession, num_peers) == 0x4EC0);
	static_assert(offsetof(snSession, channel_id) == 0x5338);
	static_assert(offsetof(snSession, m_LocalOwnerIndex) == 0x5340);
	static_assert(offsetof(snSession, m_AttrIsJoinable) == 0x5444);

#pragma pack(push, 1)
	struct netPeerComplainer
	{
		enum Flags : uint8_t
		{
			HOST = 1 << 1,
		};

		/* 0x00 */ netPeerId user_peer_id;
		/* 0x08 */ uint32_t host_endpoint;
		PAD(0x0C, 0x10) netConnectionManager* con_mgr;
		PAD(0x18, 0x258) int num_slots; // = player count - 1
		PAD(0x258 + 4, 0xE84) uint32_t channel_index; // 13 as of b2545
		PAD(0xE84 + 4, 0xE94) uint8_t flags;
		PAD(0xE94 + 1, 0xE98);

		//void sendComplaint(const netComplaintMsg* packet) noexcept;
	};
	static_assert(sizeof(netPeerComplainer) == 0xE98); // 3337
	static_assert(offsetof(netPeerComplainer, host_endpoint) == 0x08); // 2944
	static_assert(offsetof(netPeerComplainer, con_mgr) == 0x10); // 2944
	static_assert(offsetof(netPeerComplainer, channel_index) == 0xE84); // 2944
	static_assert(offsetof(netPeerComplainer, flags) == 0xE94); // 2944
};

struct BlacklistEntry
{
	rage::rlGamerHandle gamer_handle;
	PAD(sizeof(rage::rlGamerHandle), 0x20) BlacklistEntry* next;
};

class CBlacklistedGamers
{
public:
	static constexpr auto MAX_ENTRIES = 32;

	PAD(0, 0x618) BlacklistEntry* first;

	void AddGamer(const rage::rlGamerHandle& gh) noexcept;
	[[nodiscard]] bool IsBlacklisted(const rage::rlGamerHandle& target) const noexcept;
};
#pragma pack(pop)

struct NetworkGameConfig
{
	PAD(0, 0x30) uint32_t m_MaxSlots;
	/* 0x34 */ uint32_t m_MaxPrivSlots;
	/* 0x38 */ rage::rlMatchingAttributes m_MatchingAttrs;
	PAD(0x38 + sizeof(rage::rlMatchingAttributes), 0x148);
};
static_assert(sizeof(NetworkGameConfig) == 0x148); // See imul at 84 94 81 ? ? ? ? 74 11 48 69 C0
static_assert(offsetof(NetworkGameConfig, m_MaxPrivSlots) == 0x34);

class CNetworkRecentPlayers
{
	PAD(0, 0x1930);
};
static_assert(sizeof(CNetworkRecentPlayers) == 0x1930);

enum SessionType
{
	ST_None = -1,
	ST_Physical,
	ST_Transition,
	ST_Max,
};

class CNetworkSession // within ctor: 48 C7 87 ? ? 00 00 9A 99 99 3F
{
public:
	/* 0x00 */ bool unk_added_in_1_69;
	/* 0x08 */ rage::rlSessionInfo m_SessionInfoBlob;
	/* 0xD8 */ rage::sysObfuscated_Mutate<uint32_t> m_dinputCount;
	/* 0xE8 */ rage::sysObfuscated_Mutate<uint32_t> m_dinputCheckTime;
	/* 0xF8 */ rage::snSession m_Sessions[::SessionType::ST_Max];
	/* 0xAA08 */ rage::netStatus m_Status[::SessionType::ST_Max];
	/* 0xAA18 */ rage::snSession* unk_2944_duplicate_session_pointers[::SessionType::ST_Max];
	/* 0xAA28 */ rage::netStatus* unk_2944_duplicate_session_status_pointers[::SessionType::ST_Max];
	/* 0xAA38 */ rage::snSession* session;
	/* 0xAA40 */ rage::snSession* transition;
	/* 0xAA48 */ rage::netStatus* session_status;
	/* 0xAA50 */ rage::netStatus* transition_status;
	/* 0xAA58 */ rage::netStatus modify_presence_status;
	/* 0xAA60 */ NetworkGameConfig m_Config[::SessionType::ST_Max];
	/* 0xACF0 */ bool m_bChangeAttributesPending[::SessionType::ST_Max];
	rage::netStatus m_ChangeAttributesStatus[::SessionType::ST_Max];
	/* 0xAD04 */ uint32_t m_nHostFlags[::SessionType::ST_Max];
	PAD(0xAD04 + 8, 0xB09C) uint32_t m_SessionState; // 48 8B D9 8B 89 ? ? ? ? 83 F9 08 7D
	PAD(0xB09C + 4, 0x1E54C) int m_MatchmakingGroupMax[5]; // CNetworkSession::SetMatchmakingGroupMax - 83 FA FF 74 15 48 63 C2 // 0 = regular joiner, 4 = spectator. signed ints but must not be negative?
	PAD(0x1E54C + (4 * 5), 0x2E580) int host_queue_sort_algo;
	PAD(0x2E580 + 4, 0x2E5A8) rage::netPeerComplainer complaint_mgr;
	/* 0x2E5A8 + 0xE98 */ rage::netPeerComplainer transition_complainer;
	// bool m_bAllowComplaintsWhenEstablishing;
	// CNetworkRecentPlayers m_RecentPlayers;
	PAD(0x2E5A8 + (0xE98 * 2), 0x32BD8) CBlacklistedGamers blacklist;
	PAD(0x32BD8 + sizeof(CBlacklistedGamers), 0x3490C) uint32_t m_TransitionState; // 74 ? 83 F8 05 7D
#if false
	unsigned m_TransitionStateTime;
	rage::rlSessionInfo m_TransitionJoinSessionInfo;
	rage::rlSlotType m_TransitionJoinSlotType;
	unsigned m_nTransitionJoinFlags;
	bool m_bCanRetryTransitionJoinFailure;
	bool m_bHasSetActivitySpectatorsMax;
	unsigned m_nActivityPlayersMax;
	unsigned m_nActivitySpectatorsMax;
	unsigned m_nLastActivitySlotsCheck;
	bool m_bLaunchingTransition;
	bool m_bLaunchedAsHost;
	bool m_bTransitionStartPending;
	bool m_bDoLaunchPending;
	int m_nDoLaunchCountdown;
	MsgTransitionLaunch m_sMsgLaunch;
	bool m_bLaunchOnStartedFromJoin;
	bool m_bAllowImmediateLaunchDuringJoin;
	bool m_bAllowBailForLaunchedJoins;
	bool m_bHasPendingTransitionBail;
	unsigned m_nImmediateLaunchCountdown;
	unsigned m_nImmediateLaunchCountdownTime;
	bool m_bDoNotLaunchFromJoinAsMigratedHost;
	unsigned m_TimeTokenForLaunchedTransition;
	/* 0x220C4 */ bool m_bIsActivitySession; // CNetworkSession::GetCurrentSession - 83 BB ? ? ? ? 05 7D ? 48 8B
	PAD(0x220C4 + 1, 0x382A8) bool m_bIsTransitionToGame;
	bool m_bIsTransitionToGameInSession;
	bool m_bIsTransitionToGameNotifiedSwap;
	bool m_bIsTransitionToGamePendingLeave;
	bool m_bIsTransitionToGameFromLobby;
	int m_TransitionToGameGameMode;
	/* 0x382B8 */ rage::rlGamerHandle m_hTransitionHostGamer; // 49 8D 9E ? ? ? ? 48 8B CB E8 ? ? ? ? 84 C0
#endif

	void setSlots(Stand::SessionType type) noexcept;
	void setMatchmakingGroupSlots(Stand::SessionType type) noexcept;
};
static_assert(offsetof(CNetworkSession, m_SessionState) == 0xB09C);
static_assert(offsetof(CNetworkSession, m_MatchmakingGroupMax) == 0x1E54C); // b3337: 0x1DF3C -> 0x1E54C (+0x610)
static_assert(offsetof(CNetworkSession, host_queue_sort_algo) == 0x2E580);
static_assert(offsetof(CNetworkSession, complaint_mgr) == 0x2E5A8); // b3337: 0x2DF90 -> 0x2E5A8 (+0x618)
static_assert(offsetof(CNetworkSession, blacklist) == 0x32BD8); // b3337: 0x325C0 -> 0x32BD8 (+0x618)
static_assert(offsetof(CNetworkSession, m_TransitionState) == 0x3490C); // b3337: 0x342F0 -> 0x3490C (+0x61C)
