#pragma once

#include <cstdint>
#include <unordered_map>

#include "gta_fwddecl.hpp"
#include "gta_player.hpp"
#include "typedecl.hpp"

#include "ToastConfig.hpp"

namespace Stand
{
	struct PeerData
	{
		time_t left_freemode = 0;

		bool no_removed_gamer_notify = false;
		compactplayer_t physical = MAX_PLAYERS;
		compactplayer_t breakup_from = MAX_PLAYERS;
		bool vote_kicked = false;
		bool was_timing_out = false;
		compactplayer_t love_letter_from = MAX_PLAYERS;
		compactplayer_t love_letter_to = MAX_PLAYERS;
	};

	class LeaveReasons
	{
	public:
		inline static ToastConfig toast_config{};

		inline static bool normal_enabled = true;
		inline static bool closed_game_enabled = true;
		inline static bool blocked_from_joining_enabled = false;

		static void onRemovedGamer(const rage::rlGamerInfo& gamer, int remove_reason);
		static void onTimedOut(const rage::rlGamerInfo& gamer);

		[[nodiscard]] static PeerData& getEntry(const rage::rlGamerInfo& gamer);
		[[nodiscard]] static PeerData& getEntry(rage::netPeerId peerId);
	private:
		inline static std::unordered_map<uint64_t, PeerData> peers{};

		static void conclude(const rage::rlGamerInfo& gamer, const Label& reason);
		static void conclude(const rage::rlGamerInfo& gamer, const Label& reason, const compactplayer_t other);
		static void concludeDueTo(const rage::rlGamerInfo& gamer, const std::string& reason);
		static void concludeUnknown(const rage::rlGamerInfo& gamer, const int id);

	public:
		static void forget(const rage::rlGamerInfo& gamer);
	};
}
