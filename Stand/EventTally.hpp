#pragma once

#include "AbstractPlayer.hpp"
#include "Codename.hpp"
#include "EventLabel.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct EventTally
	{
		AbstractPlayer p{};
		EventLabel label{};
		std::vector<flowevent_t> types{};
		floweventreaction_t reactions = 0;

		constexpr EventTally() noexcept = default;
		EventTally(AbstractPlayer p, flowevent_t init);

		void operator=(EventTally&& b);

		void add(flowevent_t type);
		void add(flowevent_t type, const Codename& extra_data);
		void addRaw(flowevent_t type, const std::string& extra_data);
		void addRaw(flowevent_t type, const Label& extra_data);

		void applyReactionsIn();
		void applyReactionsOut();

		[[nodiscard]] bool isInitialised() const noexcept;
		void invalidate();
	};
#pragma pack(pop)
}
