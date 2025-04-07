#pragma once

#include <cstdint>

#include "script_vm_reimpl.hpp"

namespace Stand
{
	struct Metrics
	{
		static inline uint16_t draws_this_second;
		static inline uint16_t draws_per_second;
		static inline uint16_t ticks_this_second;
		static inline uint16_t ticks_per_second;
		static inline uint16_t entity_handles_this_tick;
		static inline uint16_t entity_handles_last_tick;
#if SVM_REIMPL
		static inline uint64_t natives_game_invoked_this_tick;
#endif
		static inline uint64_t natives_game_invoked_last_tick;
		static inline uint32_t natives_stand_invoked_this_tick;
		static inline uint32_t natives_stand_invoked_last_tick;
	};
}
