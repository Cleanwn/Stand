#include "EventLabel.hpp"

namespace Stand
{
	EventLabel::EventLabel(EventLabel&& b)
		: raw_name(std::move(b.raw_name)), name(std::move(b.name))
	{
	}

	void EventLabel::operator=(EventLabel&& label)
	{
		raw_name = std::move(label.raw_name);
		name = std::move(label.name);
	}

	void EventLabel::operator=(Label&& label)
	{
		name = std::move(label);
	}

	std::string EventLabel::getName() const
	{
		return name.getLocalisedUtf8();
	}

	bool EventLabel::hasNonRawName() const
	{
		return name != raw_name;
	}
}
