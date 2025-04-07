#include "NuancedEventWithExtraData.hpp"

#include "FlowEvent.hpp"
#include "Label.hpp"

namespace Stand
{
	Label NuancedEventWithExtraData::getName() const noexcept
	{
		return FlowEvent::getName(type, extra_data);
	}
}
