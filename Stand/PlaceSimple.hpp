#pragma once

#include "Place.hpp"

#include "AbstractEntity.hpp"
#include "Util.hpp"

namespace Stand
{
	template <typename T>
	struct SortVectorDistanceAscending
	{
		inline static rage::Vector2 user_pos;

		inline bool operator() (const T& pos1, const T& pos2)
		{
			return Util::VDIST(user_pos.x, user_pos.y, pos1.x, pos1.y) <= Util::VDIST(user_pos.x, user_pos.y, pos2.x, pos2.y);
		}
	};

	template <class T, size_t S>
	class PlaceSimple : public Place
	{
	private:
		std::array<T, S> coords;

	public:
		explicit constexpr PlaceSimple(Label&& name, std::array<T, S>&& coords)
			: Place(std::move(name)), coords(std::move(coords))
		{
		}

		[[nodiscard]] bool supportsUsageByOthers() const noexcept final
		{
			return true;
		}

		[[nodiscard]] std::optional<PointOfPresence> getClosestPop(Click& click) final
		{
			const auto user_pos = click.getEffectiveIssuer().getPos();
			SortVectorDistanceAscending<T>::user_pos.x = user_pos.x;
			SortVectorDistanceAscending<T>::user_pos.y = user_pos.y;
			std::sort(coords.begin(), coords.end(), SortVectorDistanceAscending<T>());
			PointOfPresence pop;
			pop.pos = coords.at(0);
			return pop;
		}
	};
}
