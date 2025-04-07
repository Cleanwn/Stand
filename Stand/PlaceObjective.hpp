#pragma once

#include "PlaceBlip.hpp"

#include "natives.hpp"
#include "Util.hpp"

//#include "logger.hpp"

namespace Stand
{
	class PlaceObjective : public PlaceBlip
	{
	private:
		static constexpr std::pair<int, int> sprites[] = {
			{535, -1}, // A
			{536, -1}, // B
			{537, -1}, // C
			{538, -1}, // D
			{539, -1}, // E
			{540, -1}, // F
			{541, -1}, // G
			{542, -1}, // H
			{  1, -1}, // Standard Circle
			{432, -1}, // Headhunter Target
			{568, -1}, // Doomsday Heist Crates
			{ 64, 57}, // Mission Helicopter
			{501, 57}, // MC Supplies
			{556, 57}, // Bunker Supplies
			{225, 57}, // Car
			{280, 57}, // Person that wants to be picked up 
			{631, 57}, // Nightclub Music Equipment
			{760, 60}, // Kosatka, Yellow
			{765,  2}, // Cayo Perico Heist Equipment, Green
			{459,  2}, // Sightseer Package, Green
		};

	public:
		constexpr PlaceObjective()
			: PlaceBlip(LOC("OBJCTV"))
		{
		}

		std::vector<Blip> get_blips() const final
		{
			std::vector<Blip> blips;
			std::vector<Blip> entity_blips;
			for (const std::pair<int, int>& sprite : sprites)
			{
				Blip blip = HUD::GET_FIRST_BLIP_INFO_ID(sprite.first);
				while (blip != 0)
				{
					//g_logger.log(fmt::format("{}: {}", sprite.first, HUD::GET_BLIP_COLOUR(blip)));
					if (sprite.second == -1 || HUD::GET_BLIP_COLOUR(blip) == sprite.second)
					{
						Entity blip_ent = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(blip);
						if (blip_ent != 0)
						{
							if (!ENTITY::IS_ENTITY_A_PED(blip_ent) || !PED::IS_PED_A_PLAYER(blip_ent))
							{
								entity_blips.emplace_back(blip);
							}
						}
						else
						{
							blips.emplace_back(blip);
						}
					}
					blip = HUD::GET_NEXT_BLIP_INFO_ID(sprite.first);
				}
			}
			return blips.empty() ? entity_blips : blips;
		}
	};
}
