#pragma once

#include "rl.hpp"
#include "rl_matching.hpp"

namespace rage
{
	struct rlSessionConfig
	{
		/* 0x00 */ rlNetworkMode m_NetworkMode;
		/* 0x04 */ unsigned max_public_slots;
		/* 0x08 */ unsigned max_private_slots;
		/* 0x0C */ rlMatchingAttributes m_Attrs;
		unsigned m_CreateFlags;
		unsigned m_PresenceFlags;
		uint64_t session_id;
		uint64_t arbitration_cookie;
	};
	static_assert(offsetof(rlSessionConfig, m_Attrs) == 0x0C);
}
