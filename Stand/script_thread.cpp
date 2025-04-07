#include "script_thread.hpp"

#include "AbstractPlayer.hpp"
#include "atArray.hpp"
#include "CNetGamePlayer.hpp"
#include "get_current_time_millis.hpp"
#include "Label.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "script_handler.hpp"
#include "script_program.hpp"

using namespace Stand;

namespace rage
{
	int64_t* scrThreadContext::getStackPtr(void* stack) noexcept
	{
		return (int64_t*)stack + (m_stack_pointer - 1);
	}

	uint8_t* scrThreadContext::getCodePtr()
	{
		rage::scrProgram* program = scrProgram::fromHash(m_script_hash);
		//auto insptr = m_instruction_pointer;
		//return (uint8_t*)(*(int64_t*)(program->m_code_blocks + (insptr >> 14)) + (insptr & 0x3FFF) - 1);
		return program->getCodePtr(m_instruction_pointer) - 1;
	}

	int64_t* scrThread::getStackPtr() noexcept
	{
		return m_context.getStackPtr(m_stack);
	}

	scrValue* scrThread::getStackPtrAsScrValue() noexcept
	{
		return reinterpret_cast<scrValue*>(getStackPtr());
	}

	Stand::Label scrThread::getLabel() const noexcept
	{
		return getLabel(m_context.m_script_hash);
	}

	Stand::Label scrThread::getLabel(const hash_t hash) noexcept
	{
		switch (hash)
		{
			// Scripts with localised names
		case ATSTRINGHASH("MG_RACE_TO_POINT"): return LOC("SCR_FRCEPI");
		case ATSTRINGHASH("AM_AMMO_DROP"): return LOC("SCR_SERV_AD");
		case ATSTRINGHASH("AM_BRU_BOX"): return LOC("BST");
		case ATSTRINGHASH("AM_backup_heli"): return LOC("SCR_SERV_BH");
		case ATSTRINGHASH("AM_airstrike"): return LOC("SCR_SERV_A");
		case ATSTRINGHASH("AM_BOAT_TAXI"): return LOC("SCR_SERV_BT");
		case ATSTRINGHASH("AM_HELI_TAXI"): return LOC("SCR_SERV_HT");
		case ATSTRINGHASH("AM_ISLAND_BACKUP_HELI"): return LOC("SCR_SERV_IBH");
		case ATSTRINGHASH("fm_content_slasher"): return LOC("SCR_SLASHER");
		case ATSTRINGHASH("AM_HOT_TARGET"): return LOC("SCR_RFA_HT");
		case ATSTRINGHASH("AM_KILL_LIST"): return LOC("SCR_FE_KL");
		case ATSTRINGHASH("AM_CP_COLLECTION"): return LOC("SCR_FE_CPC");
		case ATSTRINGHASH("AM_CHALLENGES"): return LOC("SCR_FE_C");
		case ATSTRINGHASH("AM_PENNED_IN"): return LOC("SCR_FE_PI");
		case ATSTRINGHASH("AM_PASS_THE_PARCEL"): return LOC("SCR_FE_PTP");
		case ATSTRINGHASH("AM_HOT_PROPERTY"): return LOC("SCR_FE_HP");
		case ATSTRINGHASH("AM_DEAD_DROP"): return LOC("SCR_FE_DD");
		case ATSTRINGHASH("AM_KING_OF_THE_CASTLE"): return LOC("SCR_FE_KOTC");
		case ATSTRINGHASH("AM_CRIMINAL_DAMAGE"): return LOC("SCR_FE_CD");
		case ATSTRINGHASH("AM_HUNT_THE_BEAST"): return LOC("SCR_FE_HTB");
		case ATSTRINGHASH("gunslinger_arcade"): return LOC("SCR_AG_BRII");
		case ATSTRINGHASH("camhedz_arcade"): return LOC("SCR_AG_CAM");
		case ATSTRINGHASH("puzzle"): return LOC("SCR_AG_PUZ");
		case ATSTRINGHASH("road_arcade"): return LOC("SCR_AG_RAC");
		case ATSTRINGHASH("ggsm_arcade"): return LOC("SCR_AG_SM3");
		case ATSTRINGHASH("wizard_arcade"): return LOC("SCR_AG_TWR");
		case ATSTRINGHASH("AM_PLANE_TAKEDOWN"): return LOC("SCR_OFM_PT");
		case ATSTRINGHASH("AM_DISTRACT_COPS"): return LOC("SCR_OFM_DC");
		case ATSTRINGHASH("AM_DESTROY_VEH"): return LOC("SCR_OFM_DV");
		case ATSTRINGHASH("fm_content_sightseeing"): return LOC("SCR_STRIKE");
		case ATSTRINGHASH("FM_INTRO"): return LOC("SCR_FMINTRO");
		case ATSTRINGHASH("AM_Darts"): return LOC("SCR_DARTS");
		case ATSTRINGHASH("Pilot_School_MP"): return LOC("SCR_PS");
		case ATSTRINGHASH("FM_Impromptu_DM_Controler"): return LOC("SCR_IMPDM");
		case ATSTRINGHASH("BUSINESS_BATTLES"): return Label::combineWithSpace(LOC("SCR_FE_BB"), LIT(1, '1'));
		case ATSTRINGHASH("fm_content_business_battles"): return Label::combineWithSpace(LOC("SCR_FE_BB"), LIT(1, '2'));
			// Known scripts
		case ATSTRINGHASH("fm_hideout_controler"): return LOC("SCR_DPSSV"); // Gang Attack
		case ATSTRINGHASH("GB_DEATHMATCH"): return LIT("GB_DEATHMATCH"); // Executive Deathmatch
		case ATSTRINGHASH("GB_POINT_TO_POINT"): return LIT("GB_POINT_TO_POINT"); // Point to Point
		case ATSTRINGHASH("GB_YACHT_ROB"): return LIT("GB_YACHT_ROB"); // Piracy Prevention
		case ATSTRINGHASH("GB_BELLYBEAST"): return LIT("GB_BELLYBEAST"); // Asset Recovery
		case ATSTRINGHASH("GB_SIGHTSEER"): return LIT("GB_SIGHTSEER"); // Sightseer
		case ATSTRINGHASH("GB_HUNT_THE_BOSS"): return LIT("GB_HUNT_THE_BOSS"); // Executive Search
		case ATSTRINGHASH("GB_HEADHUNTER"): return LIT("GB_HEADHUNTER"); // Headhunter
		case ATSTRINGHASH("GB_CASHING_OUT"): return LIT("GB_CASHING_OUT"); // Cashing Out
			// Make sure we don't miss this one
		case ATSTRINGHASH("FM_Horde_Controler"): return LIT("FM_Horde_Controler");
			// The rest
		case ATSTRINGHASH("AM_HOLD_UP"): return LIT("AM_HOLD_UP");
		case ATSTRINGHASH("AM_JOYRIDER"): return LIT("AM_JOYRIDER");
		case ATSTRINGHASH("AM_TIME_TRIAL"): return LIT("AM_TIME_TRIAL");
		case ATSTRINGHASH("GB_LIMO_ATTACK"): return LIT("GB_LIMO_ATTACK");
		case ATSTRINGHASH("GB_STEAL_VEH"): return LIT("GB_STEAL_VEH");
		case ATSTRINGHASH("GB_TERMINATE"): return LIT("GB_TERMINATE");
		case ATSTRINGHASH("GB_FIVESTAR"): return LIT("GB_FIVESTAR");
		case ATSTRINGHASH("GB_ROB_SHOP"): return LIT("GB_ROB_SHOP");
		case ATSTRINGHASH("GB_COLLECT_MONEY"): return LIT("GB_COLLECT_MONEY");
		case ATSTRINGHASH("GB_ASSAULT"): return LIT("GB_ASSAULT");
		case ATSTRINGHASH("GB_VEH_SURV"): return LIT("GB_VEH_SURV");
		case ATSTRINGHASH("GB_FLYING_IN_STYLE"): return LIT("GB_FLYING_IN_STYLE");
		case ATSTRINGHASH("GB_FINDERSKEEPERS"): return LIT("GB_FINDERSKEEPERS");
		case ATSTRINGHASH("GB_CARJACKING"): return LIT("GB_CARJACKING");
		case ATSTRINGHASH("GB_CONTRABAND_BUY"): return LIT("GB_CONTRABAND_BUY");
		case ATSTRINGHASH("GB_CONTRABAND_SELL"): return LIT("GB_CONTRABAND_SELL");
		case ATSTRINGHASH("GB_CONTRABAND_DEFEND"): return LIT("GB_CONTRABAND_DEFEND");
		case ATSTRINGHASH("GB_AIRFREIGHT"): return LIT("GB_AIRFREIGHT");
		case ATSTRINGHASH("GB_SALVAGE"): return LIT("GB_SALVAGE");
		case ATSTRINGHASH("GB_FRAGILE_GOODS"): return LIT("GB_FRAGILE_GOODS");
		case ATSTRINGHASH("dont_cross_the_line"): return LIT("dont_cross_the_line");
		case ATSTRINGHASH("grid_arcade_cabinet"): return LIT("grid_arcade_cabinet");
		case ATSTRINGHASH("scroll_arcade_cabinet"): return LIT("scroll_arcade_cabinet");
		case ATSTRINGHASH("example_arcade"): return LIT("example_arcade");
		case ATSTRINGHASH("Degenatron Games"): return LIT("Degenatron Games");
		case ATSTRINGHASH("AM_CASINO_LIMO"): return LIT("AM_CASINO_LIMO");
		case ATSTRINGHASH("AM_CASINO_LUXURY_CAR"): return LIT("AM_CASINO_LUXURY_CAR");
		case ATSTRINGHASH("GB_VEHICLE_EXPORT"): return LIT("GB_VEHICLE_EXPORT");
		case ATSTRINGHASH("GB_BIKER_JOUST"): return LIT("GB_BIKER_JOUST");
		case ATSTRINGHASH("GB_BIKER_RACE_P2P"): return LIT("GB_BIKER_RACE_P2P");
		case ATSTRINGHASH("GB_BIKER_UNLOAD_WEAPONS"): return LIT("GB_BIKER_UNLOAD_WEAPONS");
		case ATSTRINGHASH("GB_BIKER_BAD_DEAL"): return LIT("GB_BIKER_BAD_DEAL");
		case ATSTRINGHASH("GB_BIKER_RESCUE_CONTACT"): return LIT("GB_BIKER_RESCUE_CONTACT");
		case ATSTRINGHASH("GB_BIKER_LAST_RESPECTS"): return LIT("GB_BIKER_LAST_RESPECTS");
		case ATSTRINGHASH("GB_BIKER_CONTRACT_KILLING"): return LIT("GB_BIKER_CONTRACT_KILLING");
		case ATSTRINGHASH("GB_BIKER_CONTRABAND_SELL"): return LIT("GB_BIKER_CONTRABAND_SELL");
		case ATSTRINGHASH("GB_BIKER_CONTRABAND_DEFEND"): return LIT("GB_BIKER_CONTRABAND_DEFEND");
		case ATSTRINGHASH("GB_ILLICIT_GOODS_RESUPPLY"): return LIT("GB_ILLICIT_GOODS_RESUPPLY");
		case ATSTRINGHASH("GB_BIKER_DRIVEBY_ASSASSIN"): return LIT("GB_BIKER_DRIVEBY_ASSASSIN");
		case ATSTRINGHASH("GB_BIKER_CRIMINAL_MISCHIEF"): return LIT("GB_BIKER_CRIMINAL_MISCHIEF");
		case ATSTRINGHASH("GB_BIKER_RIPPIN_IT_UP"): return LIT("GB_BIKER_RIPPIN_IT_UP");
		case ATSTRINGHASH("GB_PLOUGHED"): return LIT("GB_PLOUGHED");
		case ATSTRINGHASH("GB_FULLY_LOADED"): return LIT("GB_FULLY_LOADED");
		case ATSTRINGHASH("GB_AMPHIBIOUS_ASSAULT"): return LIT("GB_AMPHIBIOUS_ASSAULT");
		case ATSTRINGHASH("GB_TRANSPORTER"): return LIT("GB_TRANSPORTER");
		case ATSTRINGHASH("GB_FORTIFIED"): return LIT("GB_FORTIFIED");
		case ATSTRINGHASH("GB_VELOCITY"): return LIT("GB_VELOCITY");
		case ATSTRINGHASH("GB_RAMPED_UP"): return LIT("GB_RAMPED_UP");
		case ATSTRINGHASH("GB_STOCKPILING"): return LIT("GB_STOCKPILING");
		case ATSTRINGHASH("GB_BIKER_FREE_PRISONER"): return LIT("GB_BIKER_FREE_PRISONER");
		case ATSTRINGHASH("GB_BIKER_SAFECRACKER"): return LIT("GB_BIKER_SAFECRACKER");
		case ATSTRINGHASH("GB_BIKER_STEAL_BIKES"): return LIT("GB_BIKER_STEAL_BIKES");
		case ATSTRINGHASH("GB_BIKER_SEARCH_AND_DESTROY"): return LIT("GB_BIKER_SEARCH_AND_DESTROY");
		case ATSTRINGHASH("GB_BIKER_STAND_YOUR_GROUND"): return LIT("GB_BIKER_STAND_YOUR_GROUND");
		case ATSTRINGHASH("GB_BIKER_DESTROY_VANS"): return LIT("GB_BIKER_DESTROY_VANS");
		case ATSTRINGHASH("GB_BIKER_BURN_ASSETS"): return LIT("GB_BIKER_BURN_ASSETS");
		case ATSTRINGHASH("GB_BIKER_SHUTTLE"): return LIT("GB_BIKER_SHUTTLE");
		case ATSTRINGHASH("GB_BIKER_WHEELIE_RIDER"): return LIT("GB_BIKER_WHEELIE_RIDER");
		case ATSTRINGHASH("GB_GUNRUNNING"): return LIT("GB_GUNRUNNING");
		case ATSTRINGHASH("GB_GUNRUNNING_DEFEND"): return LIT("GB_GUNRUNNING_DEFEND");
		case ATSTRINGHASH("GB_SMUGGLER"): return LIT("GB_SMUGGLER");
		case ATSTRINGHASH("GB_GANGOPS"): return LIT("GB_GANGOPS");
		case ATSTRINGHASH("BUSINESS_BATTLES_SELL"): return LIT("BUSINESS_BATTLES_SELL");
		case ATSTRINGHASH("BUSINESS_BATTLES_DEFEND"): return LIT("BUSINESS_BATTLES_DEFEND");
		case ATSTRINGHASH("GB_SECURITY_VAN"): return LIT("GB_SECURITY_VAN");
		case ATSTRINGHASH("GB_TARGET_PURSUIT"): return LIT("GB_TARGET_PURSUIT");
		case ATSTRINGHASH("GB_JEWEL_STORE_GRAB"): return LIT("GB_JEWEL_STORE_GRAB");
		case ATSTRINGHASH("GB_BANK_JOB"): return LIT("GB_BANK_JOB");
		case ATSTRINGHASH("GB_DATA_HACK"): return LIT("GB_DATA_HACK");
		case ATSTRINGHASH("GB_INFILTRATION"): return LIT("GB_INFILTRATION");
		case ATSTRINGHASH("GB_CASINO"): return LIT("GB_CASINO");
		case ATSTRINGHASH("GB_CASINO_HEIST"): return LIT("GB_CASINO_HEIST");
		case ATSTRINGHASH("fm_content_crime_scene"): return LIT("fm_content_crime_scene");
		case ATSTRINGHASH("fm_content_drug_vehicle"): return LIT("fm_content_drug_vehicle");
		case ATSTRINGHASH("fm_content_movie_props"): return LIT("fm_content_movie_props");
		case ATSTRINGHASH("fm_content_island_heist"): return LIT("fm_content_island_heist");
		case ATSTRINGHASH("fm_content_island_dj"): return LIT("fm_content_island_dj");
		case ATSTRINGHASH("fm_content_golden_gun"): return LIT("fm_content_golden_gun");
		case ATSTRINGHASH("AM_CR_SELL_DRUGS"): return LIT("AM_CR_SELL_DRUGS");
		case ATSTRINGHASH("AM_Safehouse"): return LIT("AM_Safehouse");
		case ATSTRINGHASH("AM_CRATE_DROP"): return LIT("AM_CRATE_DROP");
		case ATSTRINGHASH("AM_VEHICLE_DROP"): return LIT("AM_VEHICLE_DROP");
		case ATSTRINGHASH("AM_GA_PICKUPS"): return LIT("AM_GA_PICKUPS");
		case ATSTRINGHASH("AM_PI_MENU"): return LIT("AM_PI_MENU");
		case ATSTRINGHASH("AM_IMP_EXP"): return LIT("AM_IMP_EXP");
		case ATSTRINGHASH("AM_TAXI"): return LIT("AM_TAXI");
		case ATSTRINGHASH("AM_TAXI_LAUNCHER"): return LIT("AM_TAXI_LAUNCHER");
		case ATSTRINGHASH("AM_GANG_CALL"): return LIT("AM_GANG_CALL");
		case ATSTRINGHASH("heli_gun"): return LIT("heli_gun");
		case ATSTRINGHASH("am_rollercoaster"): return LIT("am_rollercoaster");
		case ATSTRINGHASH("am_ferriswheel"): return LIT("am_ferriswheel");
		case ATSTRINGHASH("AM_LAUNCHER"): return LIT("AM_LAUNCHER");
		case ATSTRINGHASH("AM_DAILY_OBJECTIVES"): return LIT("AM_DAILY_OBJECTIVES");
		case ATSTRINGHASH("AM_STRIPPER"): return LIT("AM_STRIPPER");
		case ATSTRINGHASH("AM_Hitchhiker"): return LIT("AM_Hitchhiker");
		case ATSTRINGHASH("stripclub_mp"): return LIT("stripclub_mp");
		case ATSTRINGHASH("AM_ArmWrestling"): return LIT("AM_ArmWrestling");
		case ATSTRINGHASH("AM_Tennis"): return LIT("AM_Tennis");
		case ATSTRINGHASH("AM_ImportExport"): return LIT("AM_ImportExport");
		case ATSTRINGHASH("AM_FistFight"): return LIT("AM_FistFight");
		case ATSTRINGHASH("AM_DropOffHooker"): return LIT("AM_DropOffHooker");
		case ATSTRINGHASH("AM_DOORS"): return LIT("AM_DOORS");
		case ATSTRINGHASH("AM_PROSTITUTE"): return LIT("AM_PROSTITUTE");
		case ATSTRINGHASH("fm_hold_up_tut"): return LIT("fm_hold_up_tut");
		case ATSTRINGHASH("AM_CAR_MOD_TUT"): return LIT("AM_CAR_MOD_TUT");
		case ATSTRINGHASH("AM_CONTACT_REQUESTS"): return LIT("AM_CONTACT_REQUESTS");
		case ATSTRINGHASH("am_mission_launch"): return LIT("am_mission_launch");
		case ATSTRINGHASH("am_npc_invites"): return LIT("am_npc_invites");
		case ATSTRINGHASH("am_lester_cut"): return LIT("am_lester_cut");
		case ATSTRINGHASH("AM_VEHICLE_SPAWN"): return LIT("AM_VEHICLE_SPAWN");
		case ATSTRINGHASH("am_ronTrevor_Cut"): return LIT("am_ronTrevor_Cut");
		case ATSTRINGHASH("AM_ARMYBASE"): return LIT("AM_ARMYBASE");
		case ATSTRINGHASH("AM_PRISON"): return LIT("AM_PRISON");
		case ATSTRINGHASH("fm_Bj_race_controler"): return LIT("fm_Bj_race_controler");
		case ATSTRINGHASH("fm_deathmatch_controler"): return LIT("fm_deathmatch_controler");
		case ATSTRINGHASH("golf_mp"): return LIT("golf_mp");
		case ATSTRINGHASH("fm_mission_controller"): return LIT("fm_mission_controller");
		case ATSTRINGHASH("FM_Race_Controler"): return LIT("FM_Race_Controler");
		case ATSTRINGHASH("Range_Modern_MP"): return LIT("Range_Modern_MP");
		case ATSTRINGHASH("FM_Survival_Controller"): return LIT("FM_Survival_Controller");
		case ATSTRINGHASH("tennis_network_mp"): return LIT("tennis_network_mp");
		case ATSTRINGHASH("am_heist_int"): return LIT("am_heist_int");
		case ATSTRINGHASH("am_lowrider_int"): return LIT("am_lowrider_int");
		case ATSTRINGHASH("am_darts_apartment"): return LIT("am_darts_apartment");
		case ATSTRINGHASH("AM_Armwrestling_Apartment"): return LIT("AM_Armwrestling_Apartment");
		case ATSTRINGHASH("SCTV"): return LIT("SCTV");
		case ATSTRINGHASH("fm_content_tuner_robbery"): return LIT("fm_content_tuner_robbery");
		case ATSTRINGHASH("fm_content_vehicle_list"): return LIT("fm_content_vehicle_list");
		case ATSTRINGHASH("tuner_sandbox_activity"): return LIT("tuner_sandbox_activity");
		case ATSTRINGHASH("fm_content_auto_shop_delivery"): return LIT("fm_content_auto_shop_delivery");
		case ATSTRINGHASH("fm_content_payphone_hit"): return LIT("fm_content_payphone_hit");
		case ATSTRINGHASH("fm_content_security_contract"): return LIT("fm_content_security_contract");
		case ATSTRINGHASH("fm_content_vip_contract_1"): return LIT("fm_content_vip_contract_1");
		case ATSTRINGHASH("fm_content_metal_detector"): return LIT("fm_content_metal_detector");
		case ATSTRINGHASH("am_agency_suv"): return LIT("am_agency_suv");
		case ATSTRINGHASH("fm_content_phantom_car"): return LIT("fm_content_phantom_car");
		case ATSTRINGHASH("fm_content_smuggler_trail"): return LIT("fm_content_smuggler_trail");
		case ATSTRINGHASH("fm_content_skydive"): return LIT("fm_content_skydive");
		case ATSTRINGHASH("fm_content_cerberus"): return LIT("fm_content_cerberus");
		case ATSTRINGHASH("fm_content_smuggler_plane"): return LIT("fm_content_smuggler_plane");
		case ATSTRINGHASH("fm_content_parachuter"): return LIT("fm_content_parachuter");
		case ATSTRINGHASH("fm_content_bar_resupply"): return LIT("fm_content_bar_resupply");
		case ATSTRINGHASH("fm_content_bike_shop_delivery"): return LIT("fm_content_bike_shop_delivery");
		case ATSTRINGHASH("fm_content_clubhouse_contracts"): return LIT("fm_content_clubhouse_contracts");
		case ATSTRINGHASH("fm_content_cargo"): return LIT("fm_content_cargo");
		case ATSTRINGHASH("fm_content_export_cargo"): return LIT("fm_content_export_cargo");
		case ATSTRINGHASH("fm_content_ammunation"): return LIT("fm_content_ammunation");
		case ATSTRINGHASH("fm_content_gunrunning"): return LIT("fm_content_gunrunning");
		case ATSTRINGHASH("fm_content_source_research"): return LIT("fm_content_source_research");
		case ATSTRINGHASH("fm_content_club_management"): return LIT("fm_content_club_management");
		case ATSTRINGHASH("fm_content_club_odd_jobs"): return LIT("fm_content_club_odd_jobs");
		case ATSTRINGHASH("fm_content_club_source"): return LIT("fm_content_club_source");
		case ATSTRINGHASH("fm_content_convoy"): return LIT("fm_content_convoy");
		case ATSTRINGHASH("fm_content_robbery"): return LIT("fm_content_robbery");
		case ATSTRINGHASH("fm_content_acid_lab_setup"): return LIT("fm_content_acid_lab_setup");
		case ATSTRINGHASH("fm_content_acid_lab_source"): return LIT("fm_content_acid_lab_source");
		case ATSTRINGHASH("fm_content_acid_lab_sell"): return LIT("fm_content_acid_lab_sell");
		case ATSTRINGHASH("fm_content_drug_lab_work"): return LIT("fm_content_drug_lab_work");
		case ATSTRINGHASH("fm_content_stash_house"): return LIT("fm_content_stash_house");
		case ATSTRINGHASH("fm_content_taxi_driver"): return LIT("fm_content_taxi_driver");
		case ATSTRINGHASH("fm_content_xmas_mugger"): return LIT("fm_content_xmas_mugger");
		case ATSTRINGHASH("fm_content_bank_shootout"): return LIT("fm_content_bank_shootout");
		case ATSTRINGHASH("fm_content_armoured_truck"): return LIT("fm_content_armoured_truck");
		case ATSTRINGHASH("fm_content_ghosthunt"): return LIT("fm_content_ghosthunt");
		case ATSTRINGHASH("fm_content_ufo_abduction"): return LIT("fm_content_ufo_abduction");
		case ATSTRINGHASH("fm_content_smuggler_sell"): return LIT("fm_content_smuggler_sell");
		case ATSTRINGHASH("fm_content_smuggler_resupply"): return LIT("fm_content_smuggler_resupply");
		case ATSTRINGHASH("fm_content_smuggler_ops"): return LIT("fm_content_smuggler_ops");
		case ATSTRINGHASH("fm_content_bicycle_time_trial"): return LIT("fm_content_bicycle_time_trial");
		case ATSTRINGHASH("fm_content_possessed_animals"): return LIT("fm_content_possessed_animals");
		case ATSTRINGHASH("fm_content_chop_shop_delivery"): return LIT("fm_content_chop_shop_delivery");
		case ATSTRINGHASH("fm_content_xmas_truck"): return LIT("fm_content_xmas_truck");
		case ATSTRINGHASH("fm_content_tow_truck_work"): return LIT("fm_content_tow_truck_work");
		case ATSTRINGHASH("fm_content_vehrob_scoping"): return LIT("fm_content_vehrob_scoping");
		case ATSTRINGHASH("fm_content_vehrob_task"): return LIT("fm_content_vehrob_task");
		case ATSTRINGHASH("fm_content_vehrob_prep"): return LIT("fm_content_vehrob_prep");
		case ATSTRINGHASH("fm_content_vehrob_disrupt"): return LIT("fm_content_vehrob_disrupt");
		case ATSTRINGHASH("fm_content_vehrob_cargo_ship"): return LIT("fm_content_vehrob_cargo_ship");
		case ATSTRINGHASH("fm_content_vehrob_police"): return LIT("fm_content_vehrob_police");
		case ATSTRINGHASH("fm_content_vehrob_arena"): return LIT("fm_content_vehrob_arena");
		case ATSTRINGHASH("fm_content_vehrob_casino_prize"): return LIT("fm_content_vehrob_casino_prize");
		case ATSTRINGHASH("fm_content_vehrob_submarine"): return LIT("fm_content_vehrob_submarine");
		case ATSTRINGHASH("fm_content_bounty_targets"): return LIT("fm_content_bounty_targets");
		case ATSTRINGHASH("fm_content_daily_bounty"): return LIT("fm_content_daily_bounty");
		case ATSTRINGHASH("fm_content_dispatch_work"): return LIT("fm_content_dispatch_work");
		case ATSTRINGHASH("fm_content_pizza_delivery"): return LIT("fm_content_pizza_delivery");
		}
		return LIT(fmt::to_string(hash));
	}

	void scrThread::spoofAs(const std::function<void()>& func)
	{
		rage::tlsContext* tls_ctx = rage::tlsContext::get();
		rage::scrThread* og_thread = tls_ctx->m_script_thread;
		tls_ctx->m_script_thread = this;
		tls_ctx->m_is_script_thread_active = true;
		func();
		tls_ctx->m_script_thread = og_thread;
		tls_ctx->m_is_script_thread_active = (og_thread != nullptr);
	}

	void scrThread::spoofAsOpt(const std::function<void()>& func)
	{
		if (this == nullptr)
		{
			func();
		}
		else
		{
			spoofAs(func);
		}
	}

	scriptHandlerNetComponent* scrThread::getNetComponent() const noexcept
	{
		if (m_handler)
		{
			return m_handler->m_net_component;
		}
		return nullptr;
	}

	::Stand::AbstractPlayer  scrThread::getHost() const
	{
		if (auto netcomp = getNetComponent())
		{
			if (netcomp->host_data != nullptr)
			{
				return netcomp->host_data->net_player->player_id;
			}
		}
		return g_player;
	}

	bool scrThread::hostedByUser() const
	{
		if (auto netcomp = getNetComponent())
		{
			return netcomp->amHost();
		}
		return false;
	}

	bool scrThread::snatchScriptHost()
	{
		if (this != nullptr)
		{
			return snatchScriptHostInner();
		}
		return false;
	}

	bool scrThread::snatchScriptHostInner()
	{
		if (auto netcomp = getNetComponent())
		{
			netcomp->takeHost();
			return true;
		}
		return false;
	}

	bool scrThread::fiberSnatchScriptHost()
	{
		if (this != nullptr)
		{
			if (hostedByUser())
			{
				return true;
			}
			if (snatchScriptHostInner())
			{
				time_t request_time = get_current_time_millis();
				do
				{
					Script::current()->yield();
					if (GtaThread::fromHash(m_context.m_script_hash) != this)
					{
						break;
					}
					if (hostedByUser())
					{
						return true;
					}
				} while (GET_MILLIS_SINCE(request_time) < 6000);
			}
		}
		return false;
	}
}

GtaThread* GtaThread::fromId(const uint32_t id)
{
	for (auto thread : *pointers::script_threads)
	{
		if (thread && thread->m_context.m_thread_id == id)
		{
			return thread;
		}
	}
	return nullptr;
}

GtaThread* GtaThread::fromHash(const int32_t hash)
{
	for (auto thread : *pointers::script_threads)
	{
		if (thread && thread->m_context.m_thread_id && thread->m_context.m_script_hash == hash)
		{
			return thread;
		}
	}
	return nullptr;
}
