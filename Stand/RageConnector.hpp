#pragma once

#include <cstdint>
#include <deque>
#include <vector>

#include "conf_netcode.hpp"
#include "gta_fwddecl.hpp"
#include "rlGamerInfo.hpp"

namespace Stand
{
#if I_CAN_SPY
	struct SpyEvent
	{
		enum Type : uint8_t
		{
			ADD_GAMER,
			REMOVE_GAMER,
		};

		Type type;
		rage::rlGamerInfo gamerInfo;
	};

	struct Spy
	{
		rage::Cxn* cxn = nullptr;
		uint64_t session_id;
		std::deque<SpyEvent> events{};

		[[nodiscard]] bool hasEvent() const noexcept;
		[[nodiscard]] SpyEvent popEvent();

		void gracefullyStopSpying();

		void fireAddGamer(rage::rlGamerInfo&& gamer);
		void fireRemoveGamer(const rage::rlGamerHandle& gamerHandle);
	};
#endif

	struct RageConnector
	{
		inline static constexpr bool use_speculative_session_preconnections = true;
		inline static uint64_t pending_join_host_startup_id = 0;

		// Connections

		[[nodiscard]] static bool isRequestActive();
		[[nodiscard]] static size_t getNumActiveRequests();
		[[nodiscard]] static bool hasConnection(const rage::netPeerId startup_id);
		[[nodiscard]] static rage::netEndpoint* getConnection(const rage::netPeerId startup_id);
		[[nodiscard]] static uint32_t getChannelId(const rage::netPeerId startup_id);
		[[nodiscard]] static bool isConnectingTo(const rage::netPeerId startup_id);
		static void connect(const rage::rlSessionInfo& si);
#if I_CAN_SPY
		static void destroyCxn(rage::Cxn* cxn);
#endif

		// Sessions

		[[nodiscard]] static rage::rlSessionDetail* getCachedSessionDetail(const rage::netPeerId startup_id);
		[[nodiscard]] static bool isSpyActive();
#if I_CAN_SPY
		[[nodiscard]] static Spy* spyFindCxnId(int32_t cxnId);
		[[nodiscard]] static Spy* spyFindSessionId(uint64_t session_id);
		static void sendJoinRequest(const rage::netPeerId host_startup_id);
		static void stopSpying(Spy* spy);
		static void stopAllSpies();
#endif
	};
}
