#include "EventTally.hpp"

#include "Exceptional.hpp"
#include "FlowEvent.hpp"

namespace Stand
{
	EventTally::EventTally(AbstractPlayer p, flowevent_t init)
		: p(p), label(FlowEvent::getName(init)), types(1, init), reactions(p.getReactions(init))
	{
	}

	void EventTally::operator=(EventTally&& b)
	{
		p = b.p;
		label = std::move(b.label);
		types = std::move(b.types);
		reactions = b.reactions;
	}

	void EventTally::add(flowevent_t type)
	{
		SOUP_IF_LIKELY (isInitialised())
		{
			this->label = FlowEvent::getName(type);
			this->types.emplace_back(type);
			this->reactions |= p.getReactions(type);
		}
#ifdef STAND_DEBUG
		else
		{
			Exceptional::report("Using an uninitalised tally");
		}
#endif
	}

	void EventTally::add(flowevent_t type, const Codename& extra_data)
	{
		addRaw(type, extra_data.toString());
	}

	void EventTally::addRaw(flowevent_t type, const std::string& extra_data)
	{
		SOUP_IF_LIKELY (isInitialised())
		{
			this->label = FlowEvent::getName(type, extra_data);
			this->types.emplace_back(type);
			this->reactions |= p.getReactions(type);
		}
#ifdef STAND_DEBUG
		else
		{
			Exceptional::report("Using an uninitalised tally");
		}
#endif
	}

	void EventTally::addRaw(flowevent_t type, const Label& extra_data)
	{
		return addRaw(type, extra_data.getLocalisedUtf8());
	}

	void EventTally::applyReactionsIn()
	{
		__try
		{
			p.applyReactionsIn(std::move(label), std::move(types), reactions);
		}
		__EXCEPTIONAL()
		{
		}
		invalidate();
	}

	void EventTally::applyReactionsOut()
	{
		p.applyReactionsOut(std::move(label), std::move(types), reactions);
		invalidate();
	}

	bool EventTally::isInitialised() const noexcept
	{
		return p.isValid();
	}

	void EventTally::invalidate()
	{
		p = AbstractPlayer::invalid();
	}
}
