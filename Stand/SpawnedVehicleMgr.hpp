#pragma once

#include "AbstractEntity.hpp"
#include "BlipUtil.hpp"
#include "CommandColour.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandToggle.hpp"
#include "get_vehicle_display_name.hpp"
#include "gta_net_object_mgr.hpp"
#include "Gui.hpp"

namespace Stand
{
	class SpawnedVehicleMgr
	{
	public:
		static inline CommandToggle* blips_cmd = nullptr;
		static inline CommandToggle* only_latest = nullptr;
		static inline CommandColour* blip_colour = nullptr;
		static inline CommandSliderFloat* blip_scale = nullptr;

		static inline std::vector<Blip> blips{};

		static void addBlip(AbstractEntity& veh)
		{
			int r, g, b;
			blip_colour->getRGB(r, g, b);

			auto blip = HUD::ADD_BLIP_FOR_ENTITY(veh);
			auto blip_name = LANG_FMT("SPWNBLIP_TXT", get_vehicle_display_name_no_manufacturer(veh.getModelHash()));

			HUD::SET_BLIP_DISPLAY(blip, 0); // Hide by default, or it might flash in first tick.
			HUD::SET_BLIP_SPRITE(blip, 225); // PV Sprite
			HUD::SET_BLIP_COLOUR(blip, 84);
			HUD::SET_BLIP_SECONDARY_COLOUR(blip, r, g, b);
			HUD::SET_BLIP_SCALE(blip, blip_scale->getFloatValue());
			HUD::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
			HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(blip_name.c_str());
			HUD::END_TEXT_COMMAND_SET_BLIP_NAME(blip);

			veh.setAlwaysSynch(true);
			blips.emplace_back(blip);
		}

		static void clearBlips()
		{
			for (auto& blip : blips)
			{
				removeBlip(blip);
			}

			blips.clear();
		}

		static void onTick()
		{
			for (auto i = blips.begin(); i != blips.end(); )
			{
				auto entity = AbstractEntity::get(HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(*i));

				if (!entity.isValid())
				{
					i = blips.erase(i);
				}
				else if ((only_latest->m_on && blips.size() > 1) || entity.isDead())
				{
					removeBlip(*i);
					i = blips.erase(i);
				}
				else
				{
					int r, g, b;
					blip_colour->getRGB(r, g, b);

					HUD::SET_BLIP_DISPLAY(*i, g_player_veh == entity ? 0 : 2);
					HUD::SET_BLIP_COLOUR(*i, 84);
					HUD::SET_BLIP_SECONDARY_COLOUR(*i, r, g, b);
					HUD::SET_BLIP_SCALE(*i, blip_scale->getFloatValue());

					++i;
				}
			}

			if (!blips.empty()
				&& (g_gui.isUnloadPending() || !blips_cmd->m_on)
				)
			{
				clearBlips();
			}
		}

	private:
		static void removeBlip(Blip blip)
		{
			if (auto entity = AbstractEntity::get(HUD::GET_BLIP_INFO_ID_ENTITY_INDEX(blip)); entity.isValid())
			{
				entity.setAlwaysSynch(false);
			}

			BlipUtil::remove(blip);
		}
	};
}