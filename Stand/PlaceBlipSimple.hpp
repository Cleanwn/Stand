#pragma once

#include "PlaceBlip.hpp"

#include <natives.hpp>

namespace Stand
{
	class PlaceBlipSimple : public PlaceBlip
	{
	private:
		const std::vector<int> sprites;

	public:
		explicit PlaceBlipSimple(Label&& name, const std::vector<int> sprites)
			: PlaceBlip(std::move(name)), sprites(sprites)
		{
		}

		std::vector<Blip> get_blips() const final
		{
			std::vector<Blip> blips;
			for (const int& sprite : sprites)
			{
				Blip blip = HUD::GET_FIRST_BLIP_INFO_ID(sprite);
				while (blip != 0)
				{
					blips.emplace_back(blip);
					blip = HUD::GET_NEXT_BLIP_INFO_ID(sprite);
				}
			}
			return blips;
		}
	};
}
