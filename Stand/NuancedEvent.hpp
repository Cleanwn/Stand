#pragma once

#include "fwddecl.hpp"
#include "typedecl.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct NuancedEvent
	{
		flowevent_t type;
		uint8_t severity;

		constexpr NuancedEvent(flowevent_t type, uint8_t severity = 100) noexcept
			: type(type), severity(severity)
		{
		}

		[[nodiscard]] Label getName() const noexcept;

		[[nodiscard]] bool isPreemptive() const noexcept;
	};
#pragma pack(pop)
}
