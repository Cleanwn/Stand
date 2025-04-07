#pragma once

#include <cstdint>

#include "struct_base.hpp"

#include "gta_fwddecl.hpp"

#include "gta_node_list.hpp"
#include "script_id.hpp"

#pragma pack(push, 1)
namespace rage
{
	class scriptHandlerNetComponent
	{
	public:
		class participantData
		{
			PAD(0x00, 0x10) class CNetGamePlayer* net_player;
			PAD(0x18, 0x1A) uint8_t physical_id;
		};
		static_assert(sizeof(participantData) == 0x1A + 1);

		virtual ~scriptHandlerNetComponent() = default; // 0x00
		virtual void Update() = 0; // 0x08
		virtual void PlayerHasJoined(netPlayer const&) = 0; // 0x10
		virtual void PlayerHasLeft(netPlayer const&) = 0; // 0x18
		virtual void MigrateScriptHost(netPlayer const&) = 0; // 0x20
		virtual void HandleJoinMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x28
		virtual void HandleJoinAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x30
		virtual void HandleJoinHostAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x38
		virtual void HandleHandshakeMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x40
		virtual void HandleHandshakeAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x48
		virtual void HandleLeaveMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x50
		virtual void HandleLeaveAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x58
		virtual void HandleBotJoinMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x60
		virtual void HandleBotJoinAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x68
		virtual void HandleBotHandshakeMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x70
		virtual void HandleBotHandshakeAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x78
		virtual void HandleBotLeaveMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x80
		virtual void HandleMigrateHostMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x88
		virtual void HandleMigrateHostFailAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x90
		virtual void HandleHostRequestMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0x98
		virtual void HandleNewHostMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0xA0
		virtual void HandleVerifyHostMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0xA8
		virtual void HandleVerifyHostAckMsg(netMessage const&, ReceivedMessageData const&) = 0; // 0xB0
		virtual void RegisterHostBroadcastData(int*, int, bool) = 0; // 0xB8
		virtual void RegisterPlayerBroadcastData(int*, int, bool) = 0; // 0xC0
		virtual bool CanStartJoining() = 0; // 0xC8
		virtual bool CanStartHosting() = 0; // 0xD0
		virtual void TryToAddPlayerAsParticipant(netPlayer const&, short&, unsigned char&, void* out_eRejectionReason) = 0; // 0xD8
		virtual void AddPlayerAsParticipant(netPlayer const&, short, unsigned char) = 0; // 0xE0
		virtual void RemovePlayerAsParticipant(netPlayer const&) = 0; // 0xE8
		virtual void HandleLeavingPlayer(netPlayer const&, bool) = 0; // 0xF0
		virtual void SetNewScriptHost(netPlayer const*, unsigned short token, bool send_msgScriptNewHost) = 0; // 0xF8
		virtual void RestartJoiningProcess() = 0;
		virtual void StartPlaying() = 0;
		virtual void DoLeaveCleanup() = 0;
		virtual void DoTerminationCleanup() = 0;

		/* 0x08 */ scriptHandler* m_script_handler;
		/* 0x10 */ uint32_t state; // < 2 == joining
		/* 0x14 */ uint32_t ack_bitset;
		PAD(0x18, 0x30) participantData* host_data;
		PAD(0x38, 0x40) uint32_t participants_bitset;
		/* 0x44 */ uint16_t host_token;
		PAD(0x44 + 2, 0x48) CNetGamePlayer* host;
		/* 0x50 */ CNetGamePlayer* next_host; // if not nullptr, "Host is migrating"
		PAD(0x58, 0x60) uint32_t net_array_data_size;
		PAD(0x60 + 4, 0x68) participantData* participants[32];
		uint32_t participants_size;
		PAD(0x68 + (32 * 8) + 4, 0x170) uint32_t migration_timer;
		PAD(0x174, 0x18C) uint8_t num_participants;
		PAD(0x18D, 0x18F) uint8_t m_NumHostBroadcastData;

		[[nodiscard]] uint32_t GetNumParticipants() const noexcept
		{
			return num_participants;
		}

		[[nodiscard]] bool IsPlayerAParticipant(uint8_t physical_player_index) const noexcept
		{
			return (participants_bitset >> physical_player_index) & 1;
		}

		[[nodiscard]] bool amHost() const noexcept;
		[[nodiscard]] bool isHost(CNetGamePlayer* player) const noexcept;
		void takeHost() noexcept;
		void giveHost(CNetGamePlayer* player) noexcept; // needs yieldable script thread
		void giveHostRaw(CNetGamePlayer* player) noexcept;

		[[nodiscard]] rage::netHostBroadcastDataHandlerBase* GetHostBroadcastDataHandler(unsigned int handlerNum);
	};
	static_assert(offsetof(scriptHandlerNetComponent, m_NumHostBroadcastData) == 0x18F);

	class scriptHandler
	{
	public:
		class atDScriptObjectNode : public atDNode<scriptHandlerObject*>
		{
		};
	public:
		virtual ~scriptHandler() = default;                                                                   //  0 (0x00)

		virtual bool Update() = 0;                                                                            //  1 (0x08)

		virtual bool NetworkUpdate() = 0;                                                                     //  2 (0x10)

		virtual void Shutdown() = 0;                                                                          //  3 (0x18)

		virtual scriptIdBase& GetScriptId() = 0;                                                              //  4 (0x20)

		virtual const scriptIdBase& GetScriptId() const = 0;                                                  //  5 (0x28)

		// Returns whether the script handler belongs to a networked script.
		virtual bool RequiresANetworkComponent() = 0;                                                         //  6 (0x30)

		// Initializes the network component for the script handler.
		virtual void CreateNetworkComponent() = 0;                                                            //  7 (0x38)

		// Deletes the script handler's network component, if it exists.
		virtual void DestroyNetworkComponent() = 0;                                                           //  8 (0x40)

		// Destroys the script handler.
		virtual bool Terminate() = 0;                                                                         //  9 (0x48)

		// Adds the object to the script handler's list of objects.
		virtual void add_object(scriptHandlerObject*, bool is_network, bool is_network_and_scriptcheck) = 0;  // 10 (0x50)

		// Something related to reservations.
		virtual void _0x58(void*) = 0;                                                                        // 11 (0x58)

		virtual void register_resource(scriptResource*, void*) = 0;                                           // 12 (0x60)

		virtual void _0x68() = 0;                                                                             // 13 (0x68)

		virtual void _0x70() = 0;                                                                             // 14 (0x70)

		virtual void _0x78() = 0;                                                                             // 15 (0x78)

		virtual void _0x80() = 0;                                                                             // 16 (0x80)

		virtual void _0x88() = 0;                                                                             // 17 (0x88)

		virtual void _0x90() = 0;                                                                             // 18 (0x90)

		virtual void _0x98() = 0;                                                                             // 19 (0x98)
	public:
		void *m_0x08;                                // 0x08
		void *m_0x10;                                // 0x10
		scrThread *m_script_thread;                  // 0x18
		atDList<atDScriptObjectNode> m_objects;      // 0x20
		scriptResource *m_resource_list_head;        // 0x30
		scriptResource *m_resource_list_tail;        // 0x38
		void *m_0x40;                                // 0x40
		scriptHandlerNetComponent *m_net_component;  // 0x48
		std::uint32_t m_0x50;                        // 0x50
		std::uint32_t m_0x54;                        // 0x54
		std::uint32_t m_0x58;                        // 0x58
		std::uint32_t m_0x60;                        // 0x5C
	};
	static_assert(sizeof(rage::scriptHandler) == 0x60);
}

class CGameScriptHandler : public rage::scriptHandler
{
public:
	CGameScriptId m_script_id; // 0x60
};
static_assert(sizeof(CGameScriptHandler) == 0xA0);

class CGameScriptHandlerNetwork : public CGameScriptHandler
{
public:
	std::uint8_t m_0xA0;        // 0xA0
	std::uint8_t m_0xA1;        // 0xA1
	std::uint8_t m_0xA2;        // 0xA2
	std::uint8_t m_0xA3;        // 0xA3
	std::uint8_t m_num_players; // 0xA4
	std::uint8_t m_0xA5;        // 0xA5
	std::uint8_t m_0xA6;        // 0xA6
	std::uint8_t m_0xA7;        // 0xA7
	std::uint8_t m_0xA8;        // 0xA8
	std::uint8_t m_0xA9;        // 0xA9
	std::uint8_t m_0xAA;        // 0xAA
	std::uint8_t m_0xAB;        // 0xAB
	std::uint8_t m_0xAC;        // 0xAC
	std::uint8_t m_0xAD;        // 0xAD
	std::uint8_t m_0xAE;        // 0xAE
	std::uint8_t m_0xAF;        // 0xAF
};
static_assert(sizeof(CGameScriptHandlerNetwork) == 0xB0);

#pragma pack(pop)
