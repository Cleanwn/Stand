#include "EventAggregation.hpp"

#include "AbstractPlayer.hpp"
#include "FlowEvent.hpp"
#include "lang.hpp"

namespace Stand
{
	EventAggregation::EventAggregation(Label&& name)
		: name(std::move(name))
	{
	}

	EventAggregation::EventAggregation(flowevent_t event, Label name)
		: types{ event }, name(std::move(name))
	{
	}

	EventAggregation::EventAggregation(std::vector<flowevent_t>&& types, Label name)
		: types(std::move(types)), name(std::move(name))
	{
	}

	void EventAggregation::add(flowevent_t type)
	{
		types.emplace_back(type);
		setName(FlowEvent::getName(type));
	}

	void EventAggregation::add(flowevent_t type, const Codename& extra_data)
	{
		types.emplace_back(type);
		setName(type, extra_data);
	}

	void EventAggregation::addRaw(flowevent_t type, std::string extra_data)
	{
		types.emplace_back(type);
		setNameRaw(type, std::move(extra_data));
	}

	void EventAggregation::addSelfIdentification(std::string menu_name)
	{
		types.emplace_back(FlowEvent::MOD_ID_OTHER);
		setName(LIT(LANG_FMT("MDDRDT_SID_T", std::move(menu_name))));
	}

	void EventAggregation::setName(Label&& name)
	{
		this->name = std::move(name);
	}

	void EventAggregation::setName(flowevent_t type, const Codename& extra_data)
	{
		this->name = FlowEvent::getName(type, extra_data.toString());
	}

	void EventAggregation::setNameRaw(flowevent_t type, std::string extra_data)
	{
		this->name = FlowEvent::getName(type);
		this->extra_data = std::move(extra_data);
	}

	Label EventAggregation::getFullName() const
	{
		Label full_name = name;
		if (!extra_data.empty())
		{
			full_name = FlowEvent::getName(std::move(full_name), extra_data);
		}
		return full_name;
	}

	floweventreaction_t EventAggregation::getReactions(AbstractPlayer p) const noexcept
	{
		floweventreaction_t reactions = 0;
		if (!types.empty())
		{
			const playertype_t playertype = p.getType();
			for (const auto& type : types)
			{
				p.onFlowEvent(type, playertype);
				reactions |= p.getReactions(type, playertype);
			}
		}
		return reactions;
	}

	flowevent_t EventAggregation::getCanonicalType() const
	{
		return types.back();
	}
}
