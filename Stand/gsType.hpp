#pragma once

#include <cstdint>

enum gsType : int64_t
{
	GS_INVITE = 0,
	GS_CLOSED_FRIEND = 1,
	GS_CLOSED_CREW = 2,
	GS_CREW = 3,
	GS_SOLO = 4,
	GS_PUBLIC = 5,
};
