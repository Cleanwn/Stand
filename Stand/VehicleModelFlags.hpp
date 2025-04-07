#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "CVehicleModelInfo.hpp"
#include "hashtype.hpp"

namespace Stand
{
	class VehicleModelFlags
	{
	public:
		soup::BigBitset<25> flags;

		explicit VehicleModelFlags(const soup::BigBitset<25>* flags);

		static inline std::unordered_map<hash_t, VehicleModelFlags> originals{};
		static inline std::unordered_set<hash_t> modified{};

		static void set(CVehicleModelInfo* info, CVehicleModelInfo::Flags flag, bool value);
		static void reset(CVehicleModelInfo* info, CVehicleModelInfo::Flags flag, bool no_erase = false);
		static void reset(CVehicleModelInfo::Flags flag);
		static void resetAll();
	};
}
