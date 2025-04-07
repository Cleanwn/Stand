#pragma once

#include <cstdint>

#include "Script.hpp"

namespace rage
{
	struct netStatus
	{
		enum StatusCode : uint32_t
		{
			NONE = 0,
			PENDING = 1,
			FAILED = 2,
			SUCCEEDED = 3,
			CANCELLED = 4,
		};

		union
		{
			uint32_t m_Status;
			StatusCode m_StatusCode = NONE;
		};
		int32_t m_ResultCode{};

		[[nodiscard]] bool isPending() const noexcept
		{
			return m_StatusCode == PENDING;
		}

		void waitUntilDone() const
		{
			while (isPending())
			{
				Stand::Script::current()->yield();
			}
		}
	};
	static_assert(sizeof(netStatus) == 0x8);
}
