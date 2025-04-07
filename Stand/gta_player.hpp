#pragma once

#include <cstdint>

#define MAX_PLAYERS 32 // GTA currently allows up to 32 players in a single session
#define NUM_SPECTATOR_SLOTS 2

using compactplayer_t = uint8_t; // 8 bits are enough to store all players ids (0-31)
using player_t = uint32_t; // Internally, GTA uses 32-bit numbers for player ids
using playerbitset_t = uint32_t; // 32 bits are enough to have a "bitflag array" for all players
