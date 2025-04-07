#pragma once

#include <soup/macros.hpp> // COUNT

#define TSE(player_bitflags, args) tse_impl(args, (int)COUNT(args), player_bitflags);

namespace Stand
{
	extern void tse_impl(void* args, int args_count, unsigned int player_bitflags);
}
