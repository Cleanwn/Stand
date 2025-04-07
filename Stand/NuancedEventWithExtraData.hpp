#pragma once

#include "NuancedEvent.hpp"

#include <string>

namespace Stand
{
#pragma pack(push, 1)
	struct NuancedEventWithExtraData : public NuancedEvent
	{
		std::string extra_data{};

		NuancedEventWithExtraData(flowevent_t type, std::string&& extra_data = {}, uint8_t severity = 100)
			: NuancedEvent(type, severity), extra_data(std::move(extra_data))
		{
		}
		
		[[nodiscard]] Label getName() const noexcept;
	};
#pragma pack(pop)
}
