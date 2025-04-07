#pragma once

#include "Place.hpp"

#include "AbstractEntity.hpp"
#include "BlipUtil.hpp"
#include "distance_sort.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	class PlaceBlip : public Place
	{
	public:
		explicit constexpr PlaceBlip(Label&& name)
			: Place(std::move(name))
		{
		}

		virtual std::vector<Blip> get_blips() const = 0;

		static void distance_sort(std::vector<Blip>& blips)
		{
			const v3 user_pos = g_player_ped.getPos();
			DistanceSort<Blip>::sort(blips, [user_pos](Blip blip)->float {
				const Vector3 pos = HUD::GET_BLIP_COORDS(blip);
				return Util::VDIST(user_pos.x, user_pos.y, pos.x, pos.y);
			});
		}

		[[nodiscard]] std::optional<PointOfPresence> getClosestPop(Click& click) final
		{
			std::vector<Blip> blips = get_blips();
			distance_sort(blips);
			for (const Blip& blip : blips)
			{
				PointOfPresence pop;
				const Entity ent = HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(blip);
				if (ENTITY::IS_ENTITY_A_VEHICLE(ent))
				{
					const int passengers = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(ent);
					for (int i = -1; i < passengers; i++)
					{
						Ped ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, i, 0);
						if (ENTITY::IS_ENTITY_A_PED(ped) && PED::IS_PED_A_PLAYER(ped))
						{
							goto nextblip;
						}
					}
					pop.veh = ent;
				}
				pop.blip = blip;
				pop.pos = BlipUtil::getCoords(blip);
				return pop;
			nextblip:;
			}
			click.setResponse(LOC("BLIPNFND"));
			return std::nullopt;
		}
	};
}
