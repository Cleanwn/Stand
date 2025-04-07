#include "NuancedEvent.hpp"

#include "FlowEvent.hpp"
#include "Label.hpp"

namespace Stand
{
	Label NuancedEvent::getName() const noexcept
	{
		return FlowEvent::getName(type);
	}

	bool NuancedEvent::isPreemptive() const noexcept
	{
		return FlowEvent::is_preemptive(type, severity);
	}
}
