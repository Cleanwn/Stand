#pragma once

#include "Place.hpp"

#include "AbstractPlayer.hpp"

namespace Stand
{
	class PlaceWaypoint : public Place
	{
	public:
		constexpr PlaceWaypoint()
			: Place(LOC("WP"))
		{
		}

		[[nodiscard]] bool supportsUsageByOthers() const noexcept final
		{
			return true;
		}

		[[nodiscard]] std::optional<PointOfPresence> getClosestPop(Click& click) final
		{
			if (auto wp = click.getEffectiveIssuer().getWaypoint())
			{
				PointOfPresence pop;
				pop.pos = wp.value();
				return pop;
			}
			click.setResponse(LOC("BLIPNFND"));
			return std::nullopt;
		}
	};
}
