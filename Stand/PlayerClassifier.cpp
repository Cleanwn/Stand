#include "PlayerClassifier.hpp"

#include "Exceptional.hpp"
#include "FlowEvent.hpp"
#include "lang.hpp"
#include "Tunables.hpp"

namespace Stand
{
	bool PlayerClassifier::canRemoveEvent(const flowevent_t type)
	{
		return type != FlowEvent::MOD_ID_STAND
			&& type != FlowEvent::MOD_ID_STAND_COLOADING
			;
	}

	void PlayerClassifier::addEvent(const NuancedEvent& event)
	{
		EXCEPTIONAL_LOCK(mtx)
		auto* e = getEvent(event.type);
		if (e == nullptr)
		{
			detections.emplace_back(event.type, event.severity);
		}
		else
		{
			e->severity = event.severity;
		}
		EXCEPTIONAL_UNLOCK(mtx)
	}

	void PlayerClassifier::removeEvent(const flowevent_t type)
	{
		EXCEPTIONAL_LOCK(mtx)
		for (auto i = detections.begin(); i != detections.end(); ++i)
		{
			if (i->type == type)
			{
				detections.erase(i);
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
	}

	bool PlayerClassifier::hasEvent(const flowevent_t type) const noexcept
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		for (const auto& e : detections)
		{
			if (e.type == type)
			{
				ret = true;
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	NuancedEvent* PlayerClassifier::getEvent(const flowevent_t type) noexcept
	{
		NuancedEvent* ret = nullptr;
		EXCEPTIONAL_LOCK(mtx)
		for (auto& e : detections)
		{
			if (e.type == type)
			{
				ret = &e;
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}
	
	const NuancedEvent* PlayerClassifier::getEvent(const flowevent_t type) const noexcept
	{
		const NuancedEvent* ret = nullptr;
		EXCEPTIONAL_LOCK(mtx)
		for (auto& e : detections)
		{
			if (e.type == type)
			{
				ret = &e;
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	PlayerClassifier::Classification PlayerClassifier::classify() const noexcept
	{
		if (hasEvent(FlowEvent::MOD_T2IP)
			|| getEvent(FlowEvent::MOD_ADMINRID)
			)
		{
			return ADMIN;
		}

		const bool ignore_stand_for_classification = g_tunables.getBool(TUNAHASH("ignore_stand_for_classification"));

		uint8_t st = 0;
		EXCEPTIONAL_LOCK(mtx)
		for (const auto& d : detections)
		{
			if (ignore_stand_for_classification
				&& (d.type == FlowEvent::MOD_ID_STAND || d.type == FlowEvent::MOD_ID_STAND_COLOADING)
				)
			{
				continue;
			}
			st += d.severity;
			if (st >= 100)
			{
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		if (st >= 100)
		{
			return MODDER;
		}
		if (st >= 50)
		{
			return LIKELY_MODDER;
		}
		if (st >= 25)
		{
			return POSSIBLE_MODDER;
		}
		return NONE;
	}

	std::string PlayerClassifier::getLocalisedClassificationName(Classification classification) noexcept
	{
		switch (classification)
		{
		case PlayerClassifier::POSSIBLE_MODDER:
			return LANG_GET("CLSFN_PM");

		case PlayerClassifier::LIKELY_MODDER:
			return LANG_GET("CLSFN_LM");

		case PlayerClassifier::MODDER:
			return LANG_GET("FLAG_MOD");

#if HAS_MODAMN
		case PlayerClassifier::MODDER_OR_ADMIN:
			return LANG_GET("FLAG_MODAMN");
#endif

		case PlayerClassifier::ADMIN:
			return LANG_GET("FLAG_AMN");
		}
		return LANG_GET("NONE");
	}

	bool PlayerClassifier::hasNonPreemptiveDetectionsThatAllowAggressiveReaction() const noexcept
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		for (const auto& d : detections)
		{
			if (!d.isPreemptive()
				&& FlowEvent::can_react_aggressively(d.type)
				)
			{
				ret = true;
				break;
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}
}
