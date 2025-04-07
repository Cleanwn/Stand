#include "SessionScriptMgr.hpp"

#include "AbstractPlayer.hpp"
#include "Click.hpp"
#include "eMpMission.hpp"
#include "get_current_time_millis.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

namespace Stand
{
	bool SessionScriptMgr::startPrecheck(Click& click)
	{
		if (!is_session_started_and_transition_finished()
			|| GtaThread::fromHash(ATSTRINGHASH("am_launcher")) == nullptr
			)
		{
			click.setResponse(LOC("CMDONL"));
			return false;
		}
		return click.isUltimateEdition();
	}

	static void setScript(const hash_t hash)
	{
		ScriptGlobal(GLOBAL_AM_LAUNCHER_HOST_DATA + 3).set<int>(0);
		ScriptGlobal(GLOBAL_AM_LAUNCHER_HOST_DATA + 3 + 1).set<int>(SessionScriptMgr::hash_to_id(hash));
	}

	void SessionScriptMgr::start(const hash_t hash)
	{
		auto thread = GtaThread::fromHash(ATSTRINGHASH("am_launcher"));
		if (!thread)
		{
			return;
		}

		if (busy)
		{
			return;
		}
		busy = true;

		Util::toast(LANG_GET("RUNSCR_PREP"));

		thread->fiberSnatchScriptHost();

		// force am_launcher into running state
		ScriptGlobal(GLOBAL_AM_LAUNCHER_HOST_DATA).set<int>(1);

		// fix stucks
		{
			time_t time = get_current_time_millis();
			while (thread->m_stack)
			{
				bool all_ready = true;
				for (const auto& p : AbstractPlayer::listExcept())
				{
					if (*ScriptLocal(thread, LOCAL_LNCH_PLAYERSTATE).at(p, 3).at(2).as<int*>() == 4
						|| *ScriptLocal(thread, LOCAL_LNCH_PLAYERSTATE).at(p, 3).at(2).as<int*>() == 6
						)
					{
						//Util::toast("fix stucks", TC_SCRIPT_YIELDABLE);

						if (!(*ScriptLocal(thread, LOCAL_LNCH_PLAYERSTATE).at(p, 3).at(2).as<int*>() & (1 << 2)))
						{
							// Client has not launched script yet, so we start something that will do nothing
							setScript(ATSTRINGHASH("scroll_arcade_cabinet"));
						}
						else
						{
							// Client expects script to be running, so using something that is running
							setScript(ATSTRINGHASH("am_launcher"));
						}
						ScriptGlobal(GLOBAL_AM_LAUNCHER_HOST_DATA + 2).set<int>(7);

						all_ready = false;
						break;
					}
				}
				if (all_ready || GET_MILLIS_SINCE(time) > 3000)
				{
					break;
				}
				Script::current()->yield();
			}
		}

		/*if (very op stuck state) // restart launcher
		{
			script_global(GLOBAL_AM_LAUNCHER_HOST_DATA).set<int>(3);

			time_t time = get_current_time_millis();
			do
			{
				if (GET_MILLIS_SINCE(time) > 3000)
				{
					Util::toast(LANG_GET("GENFAIL"), TC_SCRIPT_YIELDABLE);
					busy = false;
					return;
				}
				script::get_current()->yield();
			} while (thread = GtaThread::fromHash(ATSTRINGHASH("am_launcher")), thread == nullptr);

			thread->fiberSnatchScriptHost();
		}*/

		Util::toast(LANG_FMT("RUNSCR_RUN", rage::scrThread::getLabel(hash).getLocalisedUtf8()));

		// start the script
		*ScriptGlobal(GLOBAL_AM_LAUNCHER_HOST_DATA + 1).as<int*>() |= (1 << 1);
		setScript(hash);
		ScriptGlobal(GLOBAL_AM_LAUNCHER_HOST_DATA + 2).set<int>(4);

		/*script::get_current()->yield(1000);
		if (script_global(GLOBAL_AM_LAUNCHER_HOST_DATA + 2).get<int>() == 4)
		{
			script_global(GLOBAL_AM_LAUNCHER_HOST_DATA + 2).set<int>(5);
		}*/

		busy = false;

		/*while (script_global(GLOBAL_AM_LAUNCHER_HOST_DATA + 2).get<int>() != 0 && GtaThread::fromHash(hash) != nullptr)
		{
			script::get_current()->yield();
		}

		while (GtaThread::fromHash(hash) != nullptr)
		{
			script::get_current()->yield();
		}

		Util::toast(LANG_FMT("RUNSCR_FIN"), menu_name.getLocalisedUtf8()), TC_SCRIPT_YIELDABLE);*/
	}

	int SessionScriptMgr::hash_to_id(const hash_t hash)
	{
		for (int i = 1; i < NUM_SCRIPT_IDS; ++i)
		{
			if (id_to_hash(i) == hash)
			{
				return i;
			}
		}
		return 0;
	}

	hash_t SessionScriptMgr::id_to_hash(const int id)
	{
		switch (id)
		{
		case AM_HOLD_UP: return ATSTRINGHASH("AM_HOLD_UP");
		case AM_JOYRIDER: return ATSTRINGHASH("AM_JOYRIDER");
		case AM_PLANE_TAKEDOWN: return ATSTRINGHASH("AM_PLANE_TAKEDOWN");
		case AM_DISTRACT_COPS: return ATSTRINGHASH("AM_DISTRACT_COPS");
		case AM_DESTROY_VEH: return ATSTRINGHASH("AM_DESTROY_VEH");
		case AM_HOT_TARGET: return ATSTRINGHASH("AM_HOT_TARGET");
		case AM_KILL_LIST: return ATSTRINGHASH("AM_KILL_LIST");
		case AM_TIME_TRIAL: return ATSTRINGHASH("AM_TIME_TRIAL");
		case AM_CP_COLLECTION: return ATSTRINGHASH("AM_CP_COLLECTION");
		case AM_CHALLENGES: return ATSTRINGHASH("AM_CHALLENGES");
		case AM_PENNED_IN: return ATSTRINGHASH("AM_PENNED_IN");
		case AM_PASS_THE_PARCEL: return ATSTRINGHASH("AM_PASS_THE_PARCEL");
		case AM_HOT_PROPERTY: return ATSTRINGHASH("AM_HOT_PROPERTY");
		case AM_DEAD_DROP: return ATSTRINGHASH("AM_DEAD_DROP");
		case AM_KING_OF_THE_CASTLE: return ATSTRINGHASH("AM_KING_OF_THE_CASTLE");
		case AM_CRIMINAL_DAMAGE: return ATSTRINGHASH("AM_CRIMINAL_DAMAGE");
		case AM_HUNT_THE_BEAST: return ATSTRINGHASH("AM_HUNT_THE_BEAST");
		case GB_LIMO_ATTACK: return ATSTRINGHASH("GB_LIMO_ATTACK");
		case GB_DEATHMATCH: return ATSTRINGHASH("GB_DEATHMATCH");
		case GB_STEAL_VEH: return ATSTRINGHASH("GB_STEAL_VEH");
		case GB_POINT_TO_POINT: return ATSTRINGHASH("GB_POINT_TO_POINT");
		case GB_TERMINATE: return ATSTRINGHASH("GB_TERMINATE");
		case GB_YACHT_ROB: return ATSTRINGHASH("GB_YACHT_ROB");
		case GB_BELLYBEAST: return ATSTRINGHASH("GB_BELLYBEAST");
		case GB_FIVESTAR: return ATSTRINGHASH("GB_FIVESTAR");
		case GB_ROB_SHOP: return ATSTRINGHASH("GB_ROB_SHOP");
		case GB_COLLECT_MONEY: return ATSTRINGHASH("GB_COLLECT_MONEY");
		case GB_ASSAULT: return ATSTRINGHASH("GB_ASSAULT");
		case GB_VEH_SURV: return ATSTRINGHASH("GB_VEH_SURV");
		case GB_SIGHTSEER: return ATSTRINGHASH("GB_SIGHTSEER");
		case GB_FLYING_IN_STYLE: return ATSTRINGHASH("GB_FLYING_IN_STYLE");
		case GB_FINDERSKEEPERS: return ATSTRINGHASH("GB_FINDERSKEEPERS");
		case GB_HUNT_THE_BOSS: return ATSTRINGHASH("GB_HUNT_THE_BOSS");
		case GB_CARJACKING: return ATSTRINGHASH("GB_CARJACKING");
		case GB_HEADHUNTER: return ATSTRINGHASH("GB_HEADHUNTER");
		case GB_CONTRABAND_BUY: return ATSTRINGHASH("GB_CONTRABAND_BUY");
		case GB_CONTRABAND_SELL: return ATSTRINGHASH("GB_CONTRABAND_SELL");
		case GB_CONTRABAND_DEFEND: return ATSTRINGHASH("GB_CONTRABAND_DEFEND");
		case GB_AIRFREIGHT: return ATSTRINGHASH("GB_AIRFREIGHT");
		case GB_CASHING_OUT: return ATSTRINGHASH("GB_CASHING_OUT");
		case GB_SALVAGE: return ATSTRINGHASH("GB_SALVAGE");
		case GB_FRAGILE_GOODS: return ATSTRINGHASH("GB_FRAGILE_GOODS");
		case dont_cross_the_line: return ATSTRINGHASH("dont_cross_the_line");
		case grid_arcade_cabinet: return ATSTRINGHASH("grid_arcade_cabinet");
		case scroll_arcade_cabinet: return ATSTRINGHASH("scroll_arcade_cabinet");
		case example_arcade: return ATSTRINGHASH("example_arcade");
		case road_arcade: return ATSTRINGHASH("road_arcade");
		case Degenatron_Games: return ATSTRINGHASH("Degenatron Games");
		case gunslinger_arcade: return ATSTRINGHASH("gunslinger_arcade");
		case ggsm_arcade: return ATSTRINGHASH("ggsm_arcade");
		case wizard_arcade: return ATSTRINGHASH("wizard_arcade");
		case AM_CASINO_LIMO: return ATSTRINGHASH("AM_CASINO_LIMO");
		case AM_CASINO_LUXURY_CAR: return ATSTRINGHASH("AM_CASINO_LUXURY_CAR");
		case puzzle: return ATSTRINGHASH("puzzle");
		case camhedz_arcade: return ATSTRINGHASH("camhedz_arcade");
		case GB_VEHICLE_EXPORT: return ATSTRINGHASH("GB_VEHICLE_EXPORT");
		case GB_BIKER_JOUST: return ATSTRINGHASH("GB_BIKER_JOUST");
		case GB_BIKER_RACE_P2P: return ATSTRINGHASH("GB_BIKER_RACE_P2P");
		case GB_BIKER_UNLOAD_WEAPONS: return ATSTRINGHASH("GB_BIKER_UNLOAD_WEAPONS");
		case GB_BIKER_BAD_DEAL: return ATSTRINGHASH("GB_BIKER_BAD_DEAL");
		case GB_BIKER_RESCUE_CONTACT: return ATSTRINGHASH("GB_BIKER_RESCUE_CONTACT");
		case GB_BIKER_LAST_RESPECTS: return ATSTRINGHASH("GB_BIKER_LAST_RESPECTS");
		case GB_BIKER_CONTRACT_KILLING: return ATSTRINGHASH("GB_BIKER_CONTRACT_KILLING");
		case GB_BIKER_CONTRABAND_SELL: return ATSTRINGHASH("GB_BIKER_CONTRABAND_SELL");
		case GB_BIKER_CONTRABAND_DEFEND: return ATSTRINGHASH("GB_BIKER_CONTRABAND_DEFEND");
		case GB_ILLICIT_GOODS_RESUPPLY: return ATSTRINGHASH("GB_ILLICIT_GOODS_RESUPPLY");
		case GB_BIKER_DRIVEBY_ASSASSIN: return ATSTRINGHASH("GB_BIKER_DRIVEBY_ASSASSIN");
		case GB_BIKER_CRIMINAL_MISCHIEF: return ATSTRINGHASH("GB_BIKER_CRIMINAL_MISCHIEF");
		case GB_BIKER_RIPPIN_IT_UP: return ATSTRINGHASH("GB_BIKER_RIPPIN_IT_UP");
		case GB_PLOUGHED: return ATSTRINGHASH("GB_PLOUGHED");
		case GB_FULLY_LOADED: return ATSTRINGHASH("GB_FULLY_LOADED");
		case GB_AMPHIBIOUS_ASSAULT: return ATSTRINGHASH("GB_AMPHIBIOUS_ASSAULT");
		case GB_TRANSPORTER: return ATSTRINGHASH("GB_TRANSPORTER");
		case GB_FORTIFIED: return ATSTRINGHASH("GB_FORTIFIED");
		case GB_VELOCITY: return ATSTRINGHASH("GB_VELOCITY");
		case GB_RAMPED_UP: return ATSTRINGHASH("GB_RAMPED_UP");
		case GB_STOCKPILING: return ATSTRINGHASH("GB_STOCKPILING");
		case GB_BIKER_FREE_PRISONER: return ATSTRINGHASH("GB_BIKER_FREE_PRISONER");
		case GB_BIKER_SAFECRACKER: return ATSTRINGHASH("GB_BIKER_SAFECRACKER");
		case GB_BIKER_STEAL_BIKES: return ATSTRINGHASH("GB_BIKER_STEAL_BIKES");
		case GB_BIKER_SEARCH_AND_DESTROY: return ATSTRINGHASH("GB_BIKER_SEARCH_AND_DESTROY");
		case GB_BIKER_STAND_YOUR_GROUND: return ATSTRINGHASH("GB_BIKER_STAND_YOUR_GROUND");
		case GB_BIKER_DESTROY_VANS: return ATSTRINGHASH("GB_BIKER_DESTROY_VANS");
		case GB_BIKER_BURN_ASSETS: return ATSTRINGHASH("GB_BIKER_BURN_ASSETS");
		case GB_BIKER_SHUTTLE: return ATSTRINGHASH("GB_BIKER_SHUTTLE");
		case GB_BIKER_WHEELIE_RIDER: return ATSTRINGHASH("GB_BIKER_WHEELIE_RIDER");
		case GB_GUNRUNNING: return ATSTRINGHASH("GB_GUNRUNNING");
		case GB_GUNRUNNING_DEFEND: return ATSTRINGHASH("GB_GUNRUNNING_DEFEND");
		case GB_SMUGGLER: return ATSTRINGHASH("GB_SMUGGLER");
		case GB_GANGOPS: return ATSTRINGHASH("GB_GANGOPS");
		case BUSINESS_BATTLES: return ATSTRINGHASH("BUSINESS_BATTLES");
		case BUSINESS_BATTLES_SELL: return ATSTRINGHASH("BUSINESS_BATTLES_SELL");
		case BUSINESS_BATTLES_DEFEND: return ATSTRINGHASH("BUSINESS_BATTLES_DEFEND");
		case GB_SECURITY_VAN: return ATSTRINGHASH("GB_SECURITY_VAN");
		case GB_TARGET_PURSUIT: return ATSTRINGHASH("GB_TARGET_PURSUIT");
		case GB_JEWEL_STORE_GRAB: return ATSTRINGHASH("GB_JEWEL_STORE_GRAB");
		case GB_BANK_JOB: return ATSTRINGHASH("GB_BANK_JOB");
		case GB_DATA_HACK: return ATSTRINGHASH("GB_DATA_HACK");
		case GB_INFILTRATION: return ATSTRINGHASH("GB_INFILTRATION");
		case GB_CASINO: return ATSTRINGHASH("GB_CASINO");
		case GB_CASINO_HEIST: return ATSTRINGHASH("GB_CASINO_HEIST");
		case fm_content_business_battles: return ATSTRINGHASH("fm_content_business_battles");
		case fm_content_crime_scene: return ATSTRINGHASH("fm_content_crime_scene");
		case fm_content_drug_vehicle: return ATSTRINGHASH("fm_content_drug_vehicle");
		case fm_content_movie_props: return ATSTRINGHASH("fm_content_movie_props");
		case fm_content_island_heist: return ATSTRINGHASH("fm_content_island_heist");
		case fm_content_island_dj: return ATSTRINGHASH("fm_content_island_dj");
		case fm_content_golden_gun: return ATSTRINGHASH("fm_content_golden_gun");
		case AM_CR_SELL_DRUGS: return ATSTRINGHASH("AM_CR_SELL_DRUGS");
		case AM_Safehouse: return ATSTRINGHASH("AM_Safehouse");
		case MG_RACE_TO_POINT: return ATSTRINGHASH("MG_RACE_TO_POINT");
		case AM_CRATE_DROP: return ATSTRINGHASH("AM_CRATE_DROP");
		case AM_AMMO_DROP: return ATSTRINGHASH("AM_AMMO_DROP");
		case AM_VEHICLE_DROP: return ATSTRINGHASH("AM_VEHICLE_DROP");
		case AM_BRU_BOX: return ATSTRINGHASH("AM_BRU_BOX");
		case AM_GA_PICKUPS: return ATSTRINGHASH("AM_GA_PICKUPS");
		case AM_backup_heli: return ATSTRINGHASH("AM_backup_heli");
		case AM_airstrike: return ATSTRINGHASH("AM_airstrike");
		case AM_PI_MENU: return ATSTRINGHASH("AM_PI_MENU");
		case AM_BOAT_TAXI: return ATSTRINGHASH("AM_BOAT_TAXI");
		case AM_HELI_TAXI: return ATSTRINGHASH("AM_HELI_TAXI");
		case AM_IMP_EXP: return ATSTRINGHASH("AM_IMP_EXP");
		case AM_TAXI: return ATSTRINGHASH("AM_TAXI");
		case AM_TAXI_LAUNCHER: return ATSTRINGHASH("AM_TAXI_LAUNCHER");
		case AM_GANG_CALL: return ATSTRINGHASH("AM_GANG_CALL");
		case heli_gun: return ATSTRINGHASH("heli_gun");
		case am_rollercoaster: return ATSTRINGHASH("am_rollercoaster");
		case am_ferriswheel: return ATSTRINGHASH("am_ferriswheel");
		case AM_LAUNCHER: return ATSTRINGHASH("AM_LAUNCHER");
		case AM_DAILY_OBJECTIVES: return ATSTRINGHASH("AM_DAILY_OBJECTIVES");
		case AM_STRIPPER: return ATSTRINGHASH("AM_STRIPPER");
		case AM_Hitchhiker: return ATSTRINGHASH("AM_Hitchhiker");
		case stripclub_mp: return ATSTRINGHASH("stripclub_mp");
		case AM_ArmWrestling: return ATSTRINGHASH("AM_ArmWrestling");
		case AM_Tennis: return ATSTRINGHASH("AM_Tennis");
		case AM_Darts: return ATSTRINGHASH("AM_Darts");
		case AM_ImportExport: return ATSTRINGHASH("AM_ImportExport");
		case AM_FistFight: return ATSTRINGHASH("AM_FistFight");
		case AM_DropOffHooker: return ATSTRINGHASH("AM_DropOffHooker");
		case AM_DOORS: return ATSTRINGHASH("AM_DOORS");
		case FM_INTRO: return ATSTRINGHASH("FM_INTRO");
		case AM_PROSTITUTE: return ATSTRINGHASH("AM_PROSTITUTE");
		case fm_hold_up_tut: return ATSTRINGHASH("fm_hold_up_tut");
		case AM_CAR_MOD_TUT: return ATSTRINGHASH("AM_CAR_MOD_TUT");
		case AM_CONTACT_REQUESTS: return ATSTRINGHASH("AM_CONTACT_REQUESTS");
		case am_mission_launch: return ATSTRINGHASH("am_mission_launch");
		case am_npc_invites: return ATSTRINGHASH("am_npc_invites");
		case am_lester_cut: return ATSTRINGHASH("am_lester_cut");
		case AM_VEHICLE_SPAWN: return ATSTRINGHASH("AM_VEHICLE_SPAWN");
		case am_ronTrevor_Cut: return ATSTRINGHASH("am_ronTrevor_Cut");
		case AM_ARMYBASE: return ATSTRINGHASH("AM_ARMYBASE");
		case AM_PRISON: return ATSTRINGHASH("AM_PRISON");
		case fm_Bj_race_controler: return ATSTRINGHASH("fm_Bj_race_controler");
		case fm_deathmatch_controler: return ATSTRINGHASH("fm_deathmatch_controler");
		case FM_Impromptu_DM_Controler: return ATSTRINGHASH("FM_Impromptu_DM_Controler");
		case fm_hideout_controler: return ATSTRINGHASH("fm_hideout_controler");
		case golf_mp: return ATSTRINGHASH("golf_mp");
		case Pilot_School_MP: return ATSTRINGHASH("Pilot_School_MP");
		case fm_mission_controller: return ATSTRINGHASH("fm_mission_controller");
		case FM_Race_Controler: return ATSTRINGHASH("FM_Race_Controler");
		case Range_Modern_MP: return ATSTRINGHASH("Range_Modern_MP");
		case FM_Survival_Controller: return ATSTRINGHASH("FM_Survival_Controller");
		case tennis_network_mp: return ATSTRINGHASH("tennis_network_mp");
		case am_heist_int: return ATSTRINGHASH("am_heist_int");
		case am_lowrider_int: return ATSTRINGHASH("am_lowrider_int");
		case am_darts_apartment: return ATSTRINGHASH("am_darts_apartment");
		case AM_Armwrestling_Apartment: return ATSTRINGHASH("AM_Armwrestling_Apartment");
		case SCTV: return ATSTRINGHASH("SCTV");
		case AM_ISLAND_BACKUP_HELI: return ATSTRINGHASH("AM_ISLAND_BACKUP_HELI");
		case fm_content_tuner_robbery: return ATSTRINGHASH("fm_content_tuner_robbery");
		case fm_content_vehicle_list: return ATSTRINGHASH("fm_content_vehicle_list");
		case tuner_sandbox_activity: return ATSTRINGHASH("tuner_sandbox_activity");
		case fm_content_auto_shop_delivery: return ATSTRINGHASH("fm_content_auto_shop_delivery");
		case fm_content_payphone_hit: return ATSTRINGHASH("fm_content_payphone_hit");
		case fm_content_security_contract: return ATSTRINGHASH("fm_content_security_contract");
		case fm_content_vip_contract_1: return ATSTRINGHASH("fm_content_vip_contract_1");
		case fm_content_metal_detector: return ATSTRINGHASH("fm_content_metal_detector");
		case am_agency_suv: return ATSTRINGHASH("am_agency_suv");
		case fm_content_phantom_car: return ATSTRINGHASH("fm_content_phantom_car");
		case fm_content_slasher: return ATSTRINGHASH("fm_content_slasher");
		case fm_content_sightseeing: return ATSTRINGHASH("fm_content_sightseeing");
		case fm_content_smuggler_trail: return ATSTRINGHASH("fm_content_smuggler_trail");
		case fm_content_skydive: return ATSTRINGHASH("fm_content_skydive");
		case fm_content_cerberus: return ATSTRINGHASH("fm_content_cerberus");
		case fm_content_smuggler_plane: return ATSTRINGHASH("fm_content_smuggler_plane");
		case fm_content_parachuter: return ATSTRINGHASH("fm_content_parachuter");
		case fm_content_bar_resupply: return ATSTRINGHASH("fm_content_bar_resupply");
		case fm_content_bike_shop_delivery: return ATSTRINGHASH("fm_content_bike_shop_delivery");
		case fm_content_clubhouse_contracts: return ATSTRINGHASH("fm_content_clubhouse_contracts");
		case fm_content_cargo: return ATSTRINGHASH("fm_content_cargo");
		case fm_content_export_cargo: return ATSTRINGHASH("fm_content_export_cargo");
		case fm_content_ammunation: return ATSTRINGHASH("fm_content_ammunation");
		case fm_content_gunrunning: return ATSTRINGHASH("fm_content_gunrunning");
		case fm_content_source_research: return ATSTRINGHASH("fm_content_source_research");
		case fm_content_club_management: return ATSTRINGHASH("fm_content_club_management");
		case fm_content_club_odd_jobs: return ATSTRINGHASH("fm_content_club_odd_jobs");
		case fm_content_club_source: return ATSTRINGHASH("fm_content_club_source");
		case fm_content_convoy: return ATSTRINGHASH("fm_content_convoy");
		case fm_content_robbery: return ATSTRINGHASH("fm_content_robbery");
		case fm_content_acid_lab_setup: return ATSTRINGHASH("fm_content_acid_lab_setup");
		case fm_content_acid_lab_source: return ATSTRINGHASH("fm_content_acid_lab_source");
		case fm_content_acid_lab_sell: return ATSTRINGHASH("fm_content_acid_lab_sell");
		case fm_content_drug_lab_work: return ATSTRINGHASH("fm_content_drug_lab_work");
		case fm_content_stash_house: return ATSTRINGHASH("fm_content_stash_house");
		case fm_content_taxi_driver: return ATSTRINGHASH("fm_content_taxi_driver");
		case fm_content_xmas_mugger: return ATSTRINGHASH("fm_content_xmas_mugger");
		case fm_content_bank_shootout: return ATSTRINGHASH("fm_content_bank_shootout");
		case fm_content_armoured_truck: return ATSTRINGHASH("fm_content_armoured_truck");
		case fm_content_ghosthunt: return ATSTRINGHASH("fm_content_ghosthunt");
		case fm_content_ufo_abduction: return ATSTRINGHASH("fm_content_ufo_abduction");
		case fm_content_smuggler_sell: return ATSTRINGHASH("fm_content_smuggler_sell");
		case fm_content_smuggler_resupply: return ATSTRINGHASH("fm_content_smuggler_resupply");
		case fm_content_smuggler_ops: return ATSTRINGHASH("fm_content_smuggler_ops");
		case fm_content_bicycle_time_trial: return ATSTRINGHASH("fm_content_bicycle_time_trial");
		case fm_content_possessed_animals: return ATSTRINGHASH("fm_content_possessed_animals");
		case fm_content_chop_shop_delivery: return ATSTRINGHASH("fm_content_chop_shop_delivery");
		case fm_content_xmas_truck: return ATSTRINGHASH("fm_content_xmas_truck");
		case fm_content_tow_truck_work: return ATSTRINGHASH("fm_content_tow_truck_work");
		case fm_content_vehrob_scoping: return ATSTRINGHASH("fm_content_vehrob_scoping");
		case fm_content_vehrob_task: return ATSTRINGHASH("fm_content_vehrob_task");
		case fm_content_vehrob_prep: return ATSTRINGHASH("fm_content_vehrob_prep");
		case fm_content_vehrob_disrupt: return ATSTRINGHASH("fm_content_vehrob_disrupt");
		case fm_content_vehrob_cargo_ship: return ATSTRINGHASH("fm_content_vehrob_cargo_ship");
		case fm_content_vehrob_police: return ATSTRINGHASH("fm_content_vehrob_police");
		case fm_content_vehrob_arena: return ATSTRINGHASH("fm_content_vehrob_arena");
		case fm_content_vehrob_casino_prize: return ATSTRINGHASH("fm_content_vehrob_casino_prize");
		case fm_content_vehrob_submarine: return ATSTRINGHASH("fm_content_vehrob_submarine");
		case fm_content_bounty_targets: return ATSTRINGHASH("fm_content_bounty_targets");
		case fm_content_daily_bounty: return ATSTRINGHASH("fm_content_daily_bounty");
		case fm_content_dispatch_work: return ATSTRINGHASH("fm_content_dispatch_work");
		case fm_content_pizza_delivery: return ATSTRINGHASH("fm_content_pizza_delivery");
		}
		return 0;
	}
}
