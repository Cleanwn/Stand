#pragma once

#include <cstdint>

#include "hashtype.hpp"

#define CMDFLAG_BITS 23

namespace Stand
{
	using time_t = int64_t;
	using unixtime_t = int64_t;
	constexpr time_t TIME_MAX = 0x7FFFFFFFFFFFFFFF;

	using lang_t = uint8_t;

	using cursor_t = long long;

#if CMDFLAG_BITS == 15
	using commandflags_t = uint16_t;
#elif CMDFLAG_BITS == 23
	using commandflags_t = uint32_t;
#endif

	using toast_t = uint8_t;

	using punishment_t = uint32_t;

	using netGameEventId_t = uint16_t;
	using flowevent_t = netGameEventId_t;
	using floweventreaction_t = uint32_t;

	using playertype_t = uint8_t;
	using playerflag_t = uint32_t;

	using ToggleCorrelation_t = uint8_t;
}

namespace rage
{
	// nettypes.h
	using NetworkObjectType = uint16_t;
	using ObjectId = uint16_t;
}
