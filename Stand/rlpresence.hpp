#pragma once

#include <cstdint>

namespace rage
{
	class rlScPresenceMessage
	{
	public:
		uint64_t m_PosixTimeStamp;
		const char* const m_Contents;
	};
}
