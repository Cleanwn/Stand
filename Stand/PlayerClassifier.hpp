#pragma once

#include <string>
#include <vector>

#include "conf_classifier.hpp"

#include "NuancedEvent.hpp"
#include "RecursiveSpinlock.hpp"

namespace Stand
{
	struct PlayerClassifier
	{
		enum Classification : uint8_t
		{
			NONE = 0,
			POSSIBLE_MODDER,
			LIKELY_MODDER,
			MODDER,
#if HAS_MODAMN
			MODDER_OR_ADMIN,
#endif
			ADMIN,
		};

		mutable RecursiveSpinlock mtx;
		std::vector<NuancedEvent> detections;

		[[nodiscard]] static bool canRemoveEvent(const flowevent_t type);

		void addEvent(const NuancedEvent& event);
		void removeEvent(const flowevent_t type);

		[[nodiscard]] bool hasEvent(const flowevent_t type) const noexcept;
		[[nodiscard]] NuancedEvent* getEvent(const flowevent_t type) noexcept;
		[[nodiscard]] const NuancedEvent* getEvent(const flowevent_t type) const noexcept;

		[[nodiscard]] Classification classify() const noexcept;

		[[nodiscard]] static std::string getLocalisedClassificationName(Classification classification) noexcept;

		[[nodiscard]] bool hasNonPreemptiveDetectionsThatAllowAggressiveReaction() const noexcept;
	};
}
