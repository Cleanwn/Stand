#pragma once

#include "Place.hpp"

#include "parse_coord.hpp"

namespace Stand
{
	class PlaceClipboard : public Place
	{
	public:
		constexpr PlaceClipboard()
			: Place(LOC("CBPOS"))
		{
		}

		[[nodiscard]] std::optional<PointOfPresence> getClosestPop(Click& click) final
		{
			auto coord = parse_coord(Util::getClipboardTextUTF8());
			if (coord.has_value())
			{
				PointOfPresence pop;
				pop.pos = coord.value();
				return pop;
			}
			click.setResponse(LOC("CBPOS_T"));
			return std::nullopt;
		}
	};
}
