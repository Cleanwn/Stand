#pragma once

#include "Place.hpp"

#include "vector.hpp"

namespace Stand
{
	class PlaceSingle3d : public Place
	{
	private:
		const v3 coords;

	public:
		explicit constexpr PlaceSingle3d(Label&& name, v3&& coords)
			: Place(std::move(name)), coords(std::move(coords))
		{
		}

		[[nodiscard]] bool supportsUsageByOthers() const noexcept final
		{
			return true;
		}

		[[nodiscard]] std::optional<PointOfPresence> getClosestPop(Click& click) final
		{
			PointOfPresence pop;
			pop.pos = coords;
			return pop;
		}
	};
}
