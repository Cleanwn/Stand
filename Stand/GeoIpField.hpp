#pragma once

#include <cstdint>

namespace Stand
{
	enum GeoIpField : uint8_t
	{
		GEOIP_ISP = 0,
		GEOIP_COUNTRY,
		GEOIP_STATE,
		GEOIP_CITY,
	};
}
