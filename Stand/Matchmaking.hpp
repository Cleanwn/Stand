#pragma once

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "hashtype.hpp"

#include <functional>
#include <unordered_map>

#include "netStatus.hpp"
#include "SessionAttributes.hpp"

namespace Stand
{
	struct Matchmaking
	{
		static constexpr size_t MAX_RESULTS = 1024;

		inline static rage::netStatus status;
		inline static uint32_t num_results;
		static rage::rlSessionInfo results[MAX_RESULTS];
		inline static std::unordered_map<uhash_t, SessionAttributes> session_attributes;

		[[nodiscard]] static bool isRequestActive() noexcept;
		[[nodiscard]] static bool canSendRequest() noexcept;
		static void sendRequest(const MatchmakingConfig& config);
		static void consumeResults(const std::function<void(uhash_t, const rage::rlSessionInfo&)>& consumer);
		[[nodiscard]] static SessionAttributes getSessionAttributes(uhash_t id_hash);
		static void clear() noexcept;
	};
}
