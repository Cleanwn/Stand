#include "AntiEntitySpam.hpp"

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "CObject.hpp"
#include "CPedIntelligence.hpp"
#include "get_current_time_millis.hpp"
#include "gta_entity.hpp"
#include "gta_ped.hpp"
#include "is_session.hpp"
#include "joaatToString.hpp"
#include "lang.hpp"
#include "Renderer.hpp"
#include "tbStareAtSky.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

namespace Stand
{
	void AntiEntitySpam::onDisable()
	{
		ensureStareAtSkyIsDisabled();
		antagonists.clear();
	}

	void AntiEntitySpam::onTick()
	{
		if (!enabled)
		{
			return;
		}

		uint16_t num_antagonists = 0;

		{
			EntityAccumulatorState state;
			AbstractEntity::getAllVehicles([&](AbstractEntity&& ent)
			{
				addEntity(state, ent, ent.getPointerRaw()->archetype->hash);
				CONSUMER_CONTINUE;
			});
			num_antagonists += processNumbers(state.occurrences, [](hash_t hash) -> uint16_t
			{
				switch (hash)
				{
					// These may cause CWeapon pool overflow
				case ATSTRINGHASH("chernobog"):
				case ATSTRINGHASH("hunter"):
					return 50;

					// There shouldn't be that many spawned at once.
				case ATSTRINGHASH("freight"):
					return 10;
				}
				if (AbstractModel(hash).isBigVehicle()
					|| hash == ATSTRINGHASH("towtruck")
					|| hash == ATSTRINGHASH("towtruck2")
					|| hash == ATSTRINGHASH("towtruck3")
					|| hash == ATSTRINGHASH("towtruck4")
					)
				{
					if (hash == ATSTRINGHASH("jet") && !is_session_started()) // There can be a lot of these in SP
					{
						return 20;
					}
					return 10;
				}
				if (AbstractModel(hash).isBoat())
				{
					return 25;
				}
				return 100;
			});
		}
		{
			EntityAccumulatorState state;
			AbstractEntity::getAllPeds([&](AbstractEntity&& ent)
			{
				addEntity(state, ent, ent.getPointerRaw()->archetype->hash);
				// If we have lots of angry peds, they may overflow the CEvent pool (at default pool sizes).
				if (auto intelligence = ent.getCPedRaw()->intelligence)
				{
					if (auto rg = intelligence->GetRelationshipGroup())
					{
						if (rg->m_name == ATSTRINGHASH("HATES_PLAYER"))
						{
							addEntity(state, ent, ATSTRINGHASH("attacker"));
						}
					}
				}
				CONSUMER_CONTINUE;
			});
			num_antagonists += processNumbers(state.occurrences, [](hash_t hash) -> uint16_t
			{
				switch (hash)
				{
				case ATSTRINGHASH("ig_wade"): // Only about 45 are needed to have magenta trees and game crashes
					return 15;

				case ATSTRINGHASH("a_c_rabbit_01"): // There can be a lot of these in SP
					if (!is_session_started())
					{
						return 100;
					}
					break;

				case ATSTRINGHASH("a_f_y_clubcust_01"): // Nightclub Dancefloor
				case ATSTRINGHASH("a_m_y_clubcust_01"): // Nightclub Dancefloor
				case ATSTRINGHASH("a_f_y_clubcust_02"):
				case ATSTRINGHASH("a_m_y_clubcust_02"):
				case ATSTRINGHASH("a_f_y_clubcust_03"):
				case ATSTRINGHASH("a_m_y_clubcust_03"):
				case ATSTRINGHASH("a_f_y_clubcust_04"): // Music Locker
				case ATSTRINGHASH("a_m_y_clubcust_04"): // Music Locker
				case ATSTRINGHASH("a_f_y_beach_02"): // Cayo Perico Beach Party
				case ATSTRINGHASH("a_m_y_carclub_01"): // LS Car Meet
				case ATSTRINGHASH("mp_m_boatstaff_01"): // World > Inhabitants > NPC Existence Punishments > Die + being close to Yacht
					return 200;

				case ATSTRINGHASH("mp_m_freemode_01"):
				case ATSTRINGHASH("mp_f_freemode_01"):
					// Apparently there can be quite a few of these when a race lobby starts
					return is_session_freeroam() ? 50 : 100;
				}
				// Increasing limit in activity sessions because of survival missions.
				// Just some of the peds they use (far from exhaustive): hc_gunman, g_m_y_salvagoon_01, g_m_y_ballaoring_01
				return is_session_freeroam() ? 50 : 200;
			});
		}
		{
			EntityAccumulatorState state;
			AbstractEntity::getAllObjectsIncludingPickups([&](AbstractEntity&& ent)
			{
				auto k = ent.getPointerRaw()->archetype->hash;
				
				// Broken off doors etc. are objects with the vehicle hash.
				if (AbstractModel(k).isVehicle())
				{
					k = ATSTRINGHASH("vehicle_part");
				}

				// Ped attachments
				if (AbstractModel(k).isScubaObject())
				{
					k = ATSTRINGHASH("p_parachute_s");
				}
				else switch (k)
				{
				case ATSTRINGHASH("p_parachute_s"):
				case ATSTRINGHASH("reh_p_para_bag_reh_s_01a"):
				case ATSTRINGHASH("p_para_bag_tr_s_01a"):
				case ATSTRINGHASH("p_para_bag_xmas_s"):
				case ATSTRINGHASH("lts_p_para_bag_lts_s"):
				case ATSTRINGHASH("lts_p_para_bag_pilot2_s"):
				case ATSTRINGHASH("vw_p_para_bag_vine_s"):
				case ATSTRINGHASH("physics_hat2"):
					k = ATSTRINGHASH("p_parachute_s");
					break;
				}

				addEntity(state, ent, k);
				CONSUMER_CONTINUE;
			});
			num_antagonists += processNumbers(state.occurrences, [](hash_t hash) -> uint16_t
			{
				switch (hash)
				{
				case ATSTRINGHASH("vehicle_part"):
					return 50;

				case ATSTRINGHASH("w_lr_rpg_rocket"): // Crashes at 500
					return 60;

				case ATSTRINGHASH("prop_fish_slice_01"): // Crashes at 720
				case ATSTRINGHASH("w_lr_rpg"): // Crashes at <100?
				case ATSTRINGHASH("w_lr_homing"):
					return 50;

					// Ped attachments
				case ATSTRINGHASH("p_parachute_s"):
					return 50;

					// Fragmentable objects
				case ATSTRINGHASH("prop_wallchunk_01"): // With, OBJECT::BREAK_OBJECT_FRAGMENT_CHILD(ent, 1, false) turns trees magenta at 400
				case ATSTRINGHASH("sr_prop_sr_boxpile_01"): // p1 = 1..21
				case ATSTRINGHASH("sr_prop_sr_boxpile_03"): // p1 = 1..18
					// Scenario objects
				case ATSTRINGHASH("prop_amb_donut"):
				case ATSTRINGHASH("prop_donut_02"):
				case ATSTRINGHASH("prop_fishing_rod_01"):
				case ATSTRINGHASH("prop_amb_40oz_01"):
				case ATSTRINGHASH("prop_amb_40oz_02"):
				case ATSTRINGHASH("prop_amb_40oz_03"):
				case ATSTRINGHASH("prop_tool_hammer"):
				case ATSTRINGHASH("prop_weld_torch"):
				case ATSTRINGHASH("prop_amb_beer_bottle"):
				case ATSTRINGHASH("prop_tool_broom"):
				case ATSTRINGHASH("p_amb_bagel_01"):
				case ATSTRINGHASH("prop_tool_jackham"):
				case ATSTRINGHASH("prop_beggers_sign_01"):
				case ATSTRINGHASH("prop_beggers_sign_02"):
				case ATSTRINGHASH("prop_beggers_sign_03"):
				case ATSTRINGHASH("prop_beggers_sign_04"):
				case ATSTRINGHASH("prop_curl_bar_01"):
				case ATSTRINGHASH("p_cs_clipboard"):
				case ATSTRINGHASH("prop_parking_wand_01"):
				case ATSTRINGHASH("prop_leaf_blower_01"):
				case ATSTRINGHASH("prop_acc_guitar_01"):
				case ATSTRINGHASH("prop_scn_police_torch"):
				case ATSTRINGHASH("prop_bongos_01"):
				case ATSTRINGHASH("prop_tennis_rack_01"):
				case ATSTRINGHASH("prop_rag_01"):
				case ATSTRINGHASH("prop_cs_trowel"):
				case ATSTRINGHASH("prop_cs_ciggy_01"):
				case ATSTRINGHASH("prop_barbell_01"):
				case ATSTRINGHASH("prop_barbell_02"):
					return 50;

				case ATSTRINGHASH("prop_rock_5_e"): // Doomsday Heist Act 3
				case ATSTRINGHASH("prop_rock_5_smash1"): // 2529.1895, 366.40875, 158.97002
				case ATSTRINGHASH("h4_prop_h4_glass_disp_01a"):
				case ATSTRINGHASH("h4_prop_h4_glass_disp_01b"):
				case 0x748CA531: // NVE puts a lot of whatever this is near 66.13476, -447.18546, 38.680252
					return 1000;
				}
				return 200;
			});
		}

		for (auto i = antagonists.begin(); i != antagonists.end(); )
		{
			if (GET_MILLIS_SINCE(i->second) > 3000)
			{
				Util::toast(LANG_FMT("PTX_ENTSPAM_CALM", kToString(i->first)), TOAST_ALL);
				i = antagonists.erase(i);
			}
			else
			{
				++i;
			}
		}

		if (num_antagonists >= 300
			&& use_cam
			)
		{
			/*v3 pos;
			pos.x = soup::rand(-4000.0f, 4500.0f);
			pos.y = soup::rand(-4000.0f, 8000.0f);
			pos.z = soup::rand(0.0f, 2700.0f);
			g_player_ent.setPos(pos);*/

			if (!enabled_stare_at_sky)
			{
				enabled_stare_at_sky = true;
				g_tb_stare_at_sky.enable();
				Util::toast(LOC("PTX_ENTSPAM_CAM"));
			}
		}
		else
		{
			ensureStareAtSkyIsDisabled();
		}
	}

	void AntiEntitySpam::addEntity(EntityAccumulatorState& state, AbstractEntity& ent, hash_t k)
	{
		// Track no. occurrences
		auto e = state.occurrences.find(k);
		if (e == state.occurrences.end())
		{
			state.occurrences.emplace(k, 1);
		}
		else
		{
			++e->second;
		}

		// Safely delete antagonists
		if (auto e = antagonists.find(k); e != antagonists.end())
		{
			if (ent.canBeDeleted())
			{
				bool subject_to_limits = true;
				if (ent.isObject())
				{
					if (ent.getCObject()->m_pWeapon
						&& g_player_ped.getCPed()->weapon_manager->equippedWeapon.object == ent.getCObject()
						)
					{
						return;
					}
					subject_to_limits = false;
				}
				if (subject_to_limits)
				{
					if (state.deleted_an_entity)
					{
						return;
					}
					state.deleted_an_entity = true;
				}
				ent.removeFromPlaneOfExistenceNocheck();
				e->second = get_current_time_millis();
			}
		}
	}

	uint16_t AntiEntitySpam::processNumbers(const std::unordered_map<hash_t, uint16_t>& occurrences, uint16_t(*get_treshold)(hash_t))
	{
		uint16_t num_antagonists = 0;
		for (const auto& e : occurrences)
		{
			if (e.second >= get_treshold(e.first))
			{
				const auto name = kToString(e.first);

				g_renderer.drawCentredTextThisTick(LANG_FMT("PTX_ENTSPAM_STATUS", fmt::arg("name", name), fmt::arg("count", e.second)));

				if (!antagonists.contains(e.first))
				{
					antagonists.emplace(e.first, get_current_time_millis());
					Util::toast(LANG_FMT("PTX_ENTSPAM_NEW", name), TOAST_ALL);
				}

				num_antagonists += e.second;
			}
		}
		return num_antagonists;
	}

	void AntiEntitySpam::ensureStareAtSkyIsDisabled()
	{
		if (enabled_stare_at_sky)
		{
			enabled_stare_at_sky = false;
			g_tb_stare_at_sky.disable();
		}
	}

	std::string AntiEntitySpam::kToString(hash_t k)
	{
		if (k == ATSTRINGHASH("vehicle_part"))
		{
			return LANG_GET("VEHPARTS");
		}
		if (k == ATSTRINGHASH("attacker"))
		{
			return LANG_GET("ATTKRS");
		}
		return joaatToString(k);
	}
}
