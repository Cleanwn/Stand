#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <soup/ShortString.hpp>

#include "netIpAddress.hpp"
#include "rlGamerInfo.hpp"

#include "RateLimit.hpp"

namespace Stand
{
	struct PacketRateLimit
	{
		RateLimit ratelimit;
		bool did_reaction = false;
		bool should_block = false;

		constexpr PacketRateLimit(time_t window_duration_ms, unsigned int max_window_requests)
			: ratelimit(window_duration_ms, max_window_requests)
		{
		}
	};

	class AddressGamers
	{
	public:
		struct Gamer
		{
			soup::ShortString<16> name;
			int64_t rid_2;

			void reset(const std::string& name, int64_t rid_2);

			PacketRateLimit msg_script_migrate_host_ratelimit{ 1000, 3 };
			PacketRateLimit msg_radio_station_sync_request_ratelimit{ 1000, 3 };
			PacketRateLimit msg_session_established_request_ratelimit{ 1000, 3 };
			PacketRateLimit gamer_info_request_ratelimit{ 1000, 3 };
			PacketRateLimit roaming_bubble_request_ratelimit{ 1000, 3 };
		};

		struct Peer
		{
			rage::netIpAddress ip;

			// There could be multiple gamers on the same IP address but we don't need to be that precise
			Gamer gamer;

			Peer(rage::netIpAddress ip)
				: ip(ip)
			{
			}
		};

	private:
		inline static std::vector<Peer> peers;

	public:
		[[nodiscard]] static Peer* findPeer(rage::netIpAddress ip);
		[[nodiscard]] static Peer* findOrCreatePeer(rage::netIpAddress ip);
		[[nodiscard]] static Gamer* findGamerByRid2(int64_t rid_2);

		static void onDiscoveredName(rage::netIpAddress ip, const char* name);
		static void onDiscoveredGamer(rage::netIpAddress ip, const rage::rlGamerInfo& gamerInfo);
		[[nodiscard]] static std::string getNameForUser(const rage::netIpAddress& ip);
		[[nodiscard]] static std::string getNameForUser(const Peer* peer, const rage::netIpAddress& ip);

		static void deinit();
	};
}
