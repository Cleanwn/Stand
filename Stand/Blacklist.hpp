#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "GamerIdentifier.hpp"
#include "Spinlock.hpp"

namespace Stand
{
	struct Blacklist
	{
		inline static bool has_remote_data = false;
		inline static std::unordered_map<uint64_t, uint8_t> reported_advertisers{};
		inline static Spinlock mtx{};
		inline static std::vector<soup::UniquePtr<GamerIdentifier>> advertisers{};

		[[nodiscard]] static bool isAdvertiserV2(const rage::rlGamerInfo& gi);
		[[nodiscard]] static bool isAdvertiserByRid(int64_t rid);
		static void addAdvertiser(const rage::rlGamerInfo& gi, uint8_t ad_level);
	};
}
