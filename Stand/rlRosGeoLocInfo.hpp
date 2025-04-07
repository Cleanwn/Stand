#pragma once

#include <cstdint>

namespace rage
{
	class rlRosGeoLocInfo
	{
	public:
		uint32_t region_code;
		float longitude;
		float langitude;
		bool is_secure;
		char country_code[3];
	};
}

