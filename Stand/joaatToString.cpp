#include "joaatToString.hpp"

#include <unordered_map>

#include <fmt/format.h>

#include "atStringHash.hpp"
#include "joaat_hash_db.hpp"
#include "PedModel.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"
#include "vehicle_items.hpp"
#include "weapon_components.hpp"

#define REVERSE_JOAAT(name) { ATSTRINGHASH(name), name }

namespace Stand
{
	static std::unordered_map<hash_t, const char*> joaat_map = {
		// select stats
		REVERSE_JOAAT("MPPLY_BECAME_CHEATER_NUM"),
		REVERSE_JOAAT("MPPLY_GRIEFING"),
		REVERSE_JOAAT("MPPLY_VC_ANNOYINGME"),
		REVERSE_JOAAT("MPPLY_VC_HATE"),
		REVERSE_JOAAT("MPPLY_TC_ANNOYINGME"),
		REVERSE_JOAAT("MPPLY_TC_HATE"),
		REVERSE_JOAAT("MPPLY_OFFENSIVE_LANGUAGE"),
		REVERSE_JOAAT("MPPLY_OFFENSIVE_TAGPLATE"),
		REVERSE_JOAAT("MPPLY_OFFENSIVE_UGC"),
		REVERSE_JOAAT("MPPLY_BAD_CREW_NAME"),
		REVERSE_JOAAT("MPPLY_BAD_CREW_MOTTO"),
		REVERSE_JOAAT("MPPLY_BAD_CREW_STATUS"),
		REVERSE_JOAAT("MPPLY_BAD_CREW_EMBLEM"),
		REVERSE_JOAAT("MPPLY_GAME_EXPLOITS"),
		REVERSE_JOAAT("MPPLY_EXPLOITS"),
		REVERSE_JOAAT("MPPLY_DM_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_DM_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_DM_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_RACE_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_RACE_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_RACE_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_MC_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_MC_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_MC_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_MGAME_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_MGAME_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_MGAME_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_VOTED_OUT"),
		REVERSE_JOAAT("MPPLY_VOTED_OUT_DELTA"),
		REVERSE_JOAAT("MPPLY_VOTED_OUT_QUIT"),
		REVERSE_JOAAT("MPPLY_CAP_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_CAP_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_CAP_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_SUR_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_SUR_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_SUR_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_LTS_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_LTS_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_LTS_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_PARA_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_PARA_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_PARA_CHEAT_QUIT"),
		REVERSE_JOAAT("MPPLY_FMEVN_CHEAT_START"),
		REVERSE_JOAAT("MPPLY_FMEVN_CHEAT_END"),
		REVERSE_JOAAT("MPPLY_FMEVN_CHEAT_QUIT"),
		// rewards
		REVERSE_JOAAT("REWARD_AMMO_ADVANCEDRIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_APPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_ASSAULTRIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_ASSAULTSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_ASSAULTSMG"),
		REVERSE_JOAAT("REWARD_AMMO_AUTOSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_BULLET_MP"),
		REVERSE_JOAAT("REWARD_AMMO_BULLPUPRIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_BULLPUPSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_CARBINERIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_COMBATMG"),
		REVERSE_JOAAT("REWARD_AMMO_COMBATPDW"),
		REVERSE_JOAAT("REWARD_AMMO_COMBATPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_COMPACTLAUNCHER"),
		REVERSE_JOAAT("REWARD_AMMO_COMPACTRIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_DBSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_DOUBLEACTION"),
		REVERSE_JOAAT("REWARD_AMMO_FIREWORK"),
		REVERSE_JOAAT("REWARD_AMMO_FIREWORK_MP"),
		REVERSE_JOAAT("REWARD_AMMO_FLAREGUN"),
		REVERSE_JOAAT("REWARD_AMMO_GRENADE"),
		REVERSE_JOAAT("REWARD_AMMO_GRENADELAUNCHER"),
		REVERSE_JOAAT("REWARD_AMMO_GRENADELAUNCHER_MP"),
		REVERSE_JOAAT("REWARD_AMMO_GUSENBERG"),
		REVERSE_JOAAT("REWARD_AMMO_HEAVYPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_HEAVYSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_HEAVYSNIPER"),
		REVERSE_JOAAT("REWARD_AMMO_HOMINGLAUNCHER"),
		REVERSE_JOAAT("REWARD_AMMO_MACHINEPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_MARKSMANPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_MARKSMANRIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_MG"),
		REVERSE_JOAAT("REWARD_AMMO_MICROSMG"),
		REVERSE_JOAAT("REWARD_AMMO_MINIGUN"),
		REVERSE_JOAAT("REWARD_AMMO_MINISMG"),
		REVERSE_JOAAT("REWARD_AMMO_MISSILE_MP"),
		REVERSE_JOAAT("REWARD_AMMO_MOLOTOV"),
		REVERSE_JOAAT("REWARD_AMMO_MUSKET"),
		REVERSE_JOAAT("REWARD_AMMO_PETROLCAN"),
		REVERSE_JOAAT("REWARD_AMMO_PIPEBOMB"),
		REVERSE_JOAAT("REWARD_AMMO_PISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_PISTOL50"),
		REVERSE_JOAAT("REWARD_AMMO_PROXMINE"),
		REVERSE_JOAAT("REWARD_AMMO_PUMPSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_PUMPSHOTGUN_ENTER_VEHICLE"),
		REVERSE_JOAAT("REWARD_AMMO_RAILGUN"),
		REVERSE_JOAAT("REWARD_AMMO_RAYPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_REVOLVER"),
		REVERSE_JOAAT("REWARD_AMMO_RPG"),
		REVERSE_JOAAT("REWARD_AMMO_SAWNOFFSHOTGUN"),
		REVERSE_JOAAT("REWARD_AMMO_SMG"),
		REVERSE_JOAAT("REWARD_AMMO_SMOKEGRENADE"),
		REVERSE_JOAAT("REWARD_AMMO_SNIPERRIFLE"),
		REVERSE_JOAAT("REWARD_AMMO_SNSPISTOL"),
		REVERSE_JOAAT("REWARD_AMMO_SPECIALCARBINE"),
		REVERSE_JOAAT("REWARD_AMMO_STICKYBOMB"),
		REVERSE_JOAAT("REWARD_AMMO_STUNGUN"),
		REVERSE_JOAAT("REWARD_AMMO_VINTAGEPISTOL"),
		REVERSE_JOAAT("REWARD_ARMOUR"),
		REVERSE_JOAAT("REWARD_HEALTH"),
		REVERSE_JOAAT("REWARD_HEALTH_ENTER_VEHICLE"),
		REVERSE_JOAAT("REWARD_HEALTH_VARIABLE"),
		REVERSE_JOAAT("REWARD_MONEY_VARIABLE"),
		REVERSE_JOAAT("REWARD_PARACHUTE"),
		REVERSE_JOAAT("REWARD_STAT_HEALTH"),
		REVERSE_JOAAT("REWARD_STAT_HEALTH_VARIABLE"),
		REVERSE_JOAAT("REWARD_STAT_WEAPON"),
		REVERSE_JOAAT("REWARD_VEHICLE_FIX"),
		REVERSE_JOAAT("REWARD_WEAPON_ADVANCEDRIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_APPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_ASSAULTRIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_ASSAULTRIFLE_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_ASSAULTSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_ASSAULTSMG"),
		REVERSE_JOAAT("REWARD_WEAPON_AUTOSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_BAT"),
		REVERSE_JOAAT("REWARD_WEAPON_BATTLEAXE"),
		REVERSE_JOAAT("REWARD_WEAPON_BOTTLE"),
		REVERSE_JOAAT("REWARD_WEAPON_BULLPUPRIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_BULLPUPRIFLE_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_BULLPUPSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_CARBINERIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_CARBINERIFLE_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_COMBATMG"),
		REVERSE_JOAAT("REWARD_WEAPON_COMBATMG_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_COMBATPDW"),
		REVERSE_JOAAT("REWARD_WEAPON_COMBATPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_COMPACTLAUNCHER"),
		REVERSE_JOAAT("REWARD_WEAPON_COMPACTRIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_CROWBAR"),
		REVERSE_JOAAT("REWARD_WEAPON_DAGGER"),
		REVERSE_JOAAT("REWARD_WEAPON_DBSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_DOUBLEACTION"),
		REVERSE_JOAAT("REWARD_WEAPON_FIREWORK"),
		REVERSE_JOAAT("REWARD_WEAPON_FLAREGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_FLASHLIGHT"),
		REVERSE_JOAAT("REWARD_WEAPON_GOLFCLUB"),
		REVERSE_JOAAT("REWARD_WEAPON_GRENADE"),
		REVERSE_JOAAT("REWARD_WEAPON_GRENADELAUNCHER"),
		REVERSE_JOAAT("REWARD_WEAPON_GUSENBERG"),
		REVERSE_JOAAT("REWARD_WEAPON_HAMMER"),
		REVERSE_JOAAT("REWARD_WEAPON_HATCHET"),
		REVERSE_JOAAT("REWARD_WEAPON_HEAVYPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_HEAVYSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_HEAVYSNIPER"),
		REVERSE_JOAAT("REWARD_WEAPON_HEAVYSNIPER_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_HOMINGLAUNCHER"),
		REVERSE_JOAAT("REWARD_WEAPON_KNIFE"),
		REVERSE_JOAAT("REWARD_WEAPON_KNUCKLE"),
		REVERSE_JOAAT("REWARD_WEAPON_MACHETE"),
		REVERSE_JOAAT("REWARD_WEAPON_MACHINEPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_MARKSMANPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_MARKSMANRIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_MARKSMANRIFLE_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_MG"),
		REVERSE_JOAAT("REWARD_WEAPON_MICROSMG"),
		REVERSE_JOAAT("REWARD_WEAPON_MINIGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_MINISMG"),
		REVERSE_JOAAT("REWARD_WEAPON_MOLOTOV"),
		REVERSE_JOAAT("REWARD_WEAPON_MUSKET"),
		REVERSE_JOAAT("REWARD_WEAPON_NIGHTSTICK"),
		REVERSE_JOAAT("REWARD_WEAPON_PETROLCAN"),
		REVERSE_JOAAT("REWARD_WEAPON_PIPEBOMB"),
		REVERSE_JOAAT("REWARD_WEAPON_PISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_PISTOL50"),
		REVERSE_JOAAT("REWARD_WEAPON_PISTOL_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_POOLCUE"),
		REVERSE_JOAAT("REWARD_WEAPON_PROXMINE"),
		REVERSE_JOAAT("REWARD_WEAPON_PUMPSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_PUMPSHOTGUN_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_RAILGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_RAYCARBINE"),
		REVERSE_JOAAT("REWARD_WEAPON_RAYMINIGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_RAYPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_REVOLVER"),
		REVERSE_JOAAT("REWARD_WEAPON_REVOLVER_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_RPG"),
		REVERSE_JOAAT("REWARD_WEAPON_SAWNOFFSHOTGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_SMG"),
		REVERSE_JOAAT("REWARD_WEAPON_SMG_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_SMOKEGRENADE"),
		REVERSE_JOAAT("REWARD_WEAPON_SNIPERRIFLE"),
		REVERSE_JOAAT("REWARD_WEAPON_SNSPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_SNSPISTOL_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_SPECIALCARBINE"),
		REVERSE_JOAAT("REWARD_WEAPON_SPECIALCARBINE_MK2"),
		REVERSE_JOAAT("REWARD_WEAPON_STICKYBOMB"),
		REVERSE_JOAAT("REWARD_WEAPON_STONE_HATCHET"),
		REVERSE_JOAAT("REWARD_WEAPON_STUNGUN"),
		REVERSE_JOAAT("REWARD_WEAPON_SWITCHBLADE"),
		REVERSE_JOAAT("REWARD_WEAPON_VINTAGEPISTOL"),
		REVERSE_JOAAT("REWARD_WEAPON_WRENCH"),
		// vehicle classes
		REVERSE_JOAAT("compacts"),
		REVERSE_JOAAT("sedan"),
		REVERSE_JOAAT("suv"),
		REVERSE_JOAAT("coupe"),
		REVERSE_JOAAT("muscle"),
		REVERSE_JOAAT("sport_classic"),
		REVERSE_JOAAT("sport"),
		REVERSE_JOAAT("super"),
		REVERSE_JOAAT("motorcycle"),
		REVERSE_JOAAT("off_road"),
		REVERSE_JOAAT("industrial"),
		REVERSE_JOAAT("utility"),
		REVERSE_JOAAT("van"),
		REVERSE_JOAAT("cycle"),
		REVERSE_JOAAT("boat"),
		REVERSE_JOAAT("helicopter"),
		REVERSE_JOAAT("plane"),
		REVERSE_JOAAT("service"),
		REVERSE_JOAAT("emergency"),
		REVERSE_JOAAT("military"),
		REVERSE_JOAAT("commercial"),
		REVERSE_JOAAT("rail"),
		// weapon_types
		REVERSE_JOAAT("weapon_briefcase_02"), REVERSE_JOAAT("vehicle_weapon_ruiner_bullet"), REVERSE_JOAAT("weapon_snowlauncher"), REVERSE_JOAAT("vehicle_weapon_ruiner_rocket"), REVERSE_JOAAT("weapon_cow"), REVERSE_JOAAT("weapon_combatshotgun"), REVERSE_JOAAT("weapon_sniperrifle"), REVERSE_JOAAT("weapon_fireextinguisher"), REVERSE_JOAAT("weapon_compactlauncher"), REVERSE_JOAAT("weapon_snowball"), REVERSE_JOAAT("weapon_rammed_by_car"), REVERSE_JOAAT("weapon_vintagepistol"), REVERSE_JOAAT("weapon_cougar"), REVERSE_JOAAT("vehicle_weapon_hunter_missile"), REVERSE_JOAAT("vehicle_weapon_rogue_mg"), REVERSE_JOAAT("vehicle_weapon_pounder2_missile"), REVERSE_JOAAT("weapon_combatpdw"), REVERSE_JOAAT("weapon_heavysniper_mk2"), REVERSE_JOAAT("vehicle_weapon_apc_mg"), REVERSE_JOAAT("weapon_heavysniper"), REVERSE_JOAAT("vehicle_weapon_bomb_cluster"), REVERSE_JOAAT("vehicle_weapon_scramjet_mg"), REVERSE_JOAAT("weapon_autoshotgun"), REVERSE_JOAAT("weapon_microsmg"), REVERSE_JOAAT("weapon_airstrike_rocket"), REVERSE_JOAAT("vehicle_weapon_apc_cannon"), REVERSE_JOAAT("vehicle_weapon_trailer_missile"), REVERSE_JOAAT("weapon_heli_crash"), REVERSE_JOAAT("weapon_tecpistol"), REVERSE_JOAAT("weapon_passenger_rocket"), REVERSE_JOAAT("vehicle_weapon_tula_minigun"), REVERSE_JOAAT("vehicle_weapon_khanjali_gl"), REVERSE_JOAAT("weapon_fertilizercan"), REVERSE_JOAAT("weapon_wrench"), REVERSE_JOAAT("vehicle_weapon_akula_minigun"), REVERSE_JOAAT("weapon_pistol"), REVERSE_JOAAT("weapon_pistolxm3"), REVERSE_JOAAT("vehicle_weapon_akula_turret_dual"), REVERSE_JOAAT("weapon_hackingdevice"), REVERSE_JOAAT("weapon_pumpshotgun"), REVERSE_JOAAT("vehicle_weapon_deathbike_dualminigun"), REVERSE_JOAAT("vehicle_weapon_khanjali_cannon"), REVERSE_JOAAT("vehicle_weapon_strikeforce_missile"), REVERSE_JOAAT("vehicle_weapon_barrage_rear_minigun"), REVERSE_JOAAT("weapon_explosion"), REVERSE_JOAAT("vehicle_weapon_scarab_50cal"), REVERSE_JOAAT("vehicle_weapon_zr3802_50cal_laser"), REVERSE_JOAAT("weapon_appistol"), REVERSE_JOAAT("weapon_ball"), REVERSE_JOAAT("weapon_molotov"), REVERSE_JOAAT("vehicle_weapon_mine_kinetic_rc"), REVERSE_JOAAT("vehicle_weapon_hunter_cannon"), REVERSE_JOAAT("vehicle_weapon_khanjali_mg"), REVERSE_JOAAT("weapon_ceramicpistol"), REVERSE_JOAAT("vehicle_weapon_turret_limo"), REVERSE_JOAAT("weapon_smg"), REVERSE_JOAAT("weapon_air_defence_gun"), REVERSE_JOAAT("vehicle_weapon_bombushka_dualmg"), REVERSE_JOAAT("weapon_stickybomb"), REVERSE_JOAAT("weapon_tiger_shark"), REVERSE_JOAAT("vehicle_weapon_paragon2_mg"), REVERSE_JOAAT("vehicle_weapon_hunter_barrage"), REVERSE_JOAAT("weapon_tranquilizer"), REVERSE_JOAAT("vehicle_weapon_havok_minigun"), REVERSE_JOAAT("weapon_remotesniper"), REVERSE_JOAAT("weapon_boar"), REVERSE_JOAAT("weapon_petrolcan"), REVERSE_JOAAT("weapon_arena_machine_gun"), REVERSE_JOAAT("vehicle_weapon_annihilator2_barrage"), REVERSE_JOAAT("weapon_exhaustion"), REVERSE_JOAAT("weapon_stungun"), REVERSE_JOAAT("vehicle_weapon_jb700_mg"), REVERSE_JOAAT("weapon_stone_hatchet"), REVERSE_JOAAT("vehicle_weapon_strikeforce_cannon"), REVERSE_JOAAT("weapon_hen"), REVERSE_JOAAT("weapon_assaultrifle_mk2"), REVERSE_JOAAT("object"), REVERSE_JOAAT("vehicle_weapon_strikeforce_barrage"), REVERSE_JOAAT("vehicle_weapon_slamvan4_50cal"), REVERSE_JOAAT("weapon_heavyshotgun"), REVERSE_JOAAT("vehicle_weapon_raiju_missiles"), REVERSE_JOAAT("vehicle_weapon_barrage_top_minigun"), REVERSE_JOAAT("vehicle_weapon_mine_kinetic"), REVERSE_JOAAT("vehicle_weapon_tampa_mortar"), REVERSE_JOAAT("vehicle_weapon_bruiser2_50cal_laser"), REVERSE_JOAAT("vehicle_weapon_nose_turret_valkyrie"), REVERSE_JOAAT("vehicle_weapon_tula_nosemg"), REVERSE_JOAAT("vehicle_weapon_hunter_mg"), REVERSE_JOAAT("weapon_minigun"), REVERSE_JOAAT("weapon_golfclub"), REVERSE_JOAAT("vehicle_weapon_volatol_dualmg"), REVERSE_JOAAT("vehicle_weapon_apc_missile"), REVERSE_JOAAT("vehicle_weapon_turret_insurgent"), REVERSE_JOAAT("weapon_coyote"), REVERSE_JOAAT("weapon_stungun_mp"), REVERSE_JOAAT("vehicle_weapon_subcar_mg"), REVERSE_JOAAT("vehicle_weapon_thruster_missile"), REVERSE_JOAAT("vehicle_weapon_player_buzzard"), REVERSE_JOAAT("vehicle_weapon_trailer_quadmg"), REVERSE_JOAAT("weapon_raycarbine"), REVERSE_JOAAT("vehicle_weapon_mule4_missile"), REVERSE_JOAAT("weapon_flaregun"), REVERSE_JOAAT("vehicle_weapon_barrage_rear_mg"), REVERSE_JOAAT("weapon_pig"), REVERSE_JOAAT("vehicle_weapon_tula_mg"), REVERSE_JOAAT("weapon_barbed_wire"), REVERSE_JOAAT("vehicle_weapon_halftrack_quadmg"), REVERSE_JOAAT("weapon_flare"), REVERSE_JOAAT("vehicle_weapon_player_bullet"), REVERSE_JOAAT("vehicle_weapon_patrolboat_dualmg"), REVERSE_JOAAT("weapon_grenadelauncher_smoke"), REVERSE_JOAAT("weapon_hammer"), REVERSE_JOAAT("vehicle_weapon_halftrack_dualmg"), REVERSE_JOAAT("vehicle_weapon_caracara_minigun"), REVERSE_JOAAT("vehicle_weapon_vigilante_missile"), REVERSE_JOAAT("vehicle_weapon_slamvan5_50cal_laser"), REVERSE_JOAAT("vehicle_weapon_seabreeze_mg"), REVERSE_JOAAT("vehicle_weapon_conada2_missile"), REVERSE_JOAAT("vehicle_weapon_rctank_gun"), REVERSE_JOAAT("vehicle_weapon_mine_emp_rc"), REVERSE_JOAAT("vehicle_weapon_dune_minigun"), REVERSE_JOAAT("vehicle_weapon_bomb_gas"), REVERSE_JOAAT("weapon_pumpshotgun_mk2"), REVERSE_JOAAT("vehicle_weapon_mine_slick"), REVERSE_JOAAT("weapon_gadgetpistol"), REVERSE_JOAAT("vehicle_weapon_rctank_lazer"), REVERSE_JOAAT("vehicle_weapon_mine"), REVERSE_JOAAT("vehicle_weapon_enemy_laser"), REVERSE_JOAAT("weapon_combatpistol"), REVERSE_JOAAT("vehicle_weapon_dogfighter_mg"), REVERSE_JOAAT("vehicle_weapon_impaler2_50cal"), REVERSE_JOAAT("weapon_gusenberg"), REVERSE_JOAAT("vehicle_weapon_player_savage"), REVERSE_JOAAT("weapon_compactrifle"), REVERSE_JOAAT("vehicle_weapon_kosatka_torpedo"), REVERSE_JOAAT("vehicle_weapon_mortar_kinetic"), REVERSE_JOAAT("weapon_hominglauncher"), REVERSE_JOAAT("weapon_arena_homing_missile"), REVERSE_JOAAT("vehicle_weapon_thruster_mg"), REVERSE_JOAAT("weapon_candycane"), REVERSE_JOAAT("weapon_nightstick"), REVERSE_JOAAT("vehicle_weapon_water_cannon"), REVERSE_JOAAT("vehicle_weapon_tampa_dualminigun"), REVERSE_JOAAT("weapon_stinger"), REVERSE_JOAAT("vehicle_weapon_brutus2_50cal_laser"), REVERSE_JOAAT("vehicle_weapon_mine_emp"), REVERSE_JOAAT("weapon_marksmanrifle_mk2"), REVERSE_JOAAT("vehicle_weapon_zr380_50cal"), REVERSE_JOAAT("vehicle_weapon_bomb_incendiary"), REVERSE_JOAAT("vehicle_weapon_caracara_mg"), REVERSE_JOAAT("vehicle_weapon_rogue_missile"), REVERSE_JOAAT("weapon_railgun"), REVERSE_JOAAT("weapon_bird_crap"), REVERSE_JOAAT("weapon_precisionrifle"), REVERSE_JOAAT("vehicle_weapon_bomb_standard_wide"), REVERSE_JOAAT("vehicle_weapon_streamer_nosemg"), REVERSE_JOAAT("weapon_battlerifle"), REVERSE_JOAAT("weapon_drowning_in_vehicle"), REVERSE_JOAAT("vehicle_weapon_tank"), REVERSE_JOAAT("vehicle_weapon_oppressor2_missile"), REVERSE_JOAAT("vehicle_weapon_conada2_minigun"), REVERSE_JOAAT("vehicle_weapon_issi4_50cal"), REVERSE_JOAAT("vehicle_weapon_hacker_missile"), REVERSE_JOAAT("vehicle_weapon_issi5_50cal_laser"), REVERSE_JOAAT("vehicle_weapon_rctank_rocket"), REVERSE_JOAAT("vehicle_weapon_hacker_missile_homing"), REVERSE_JOAAT("vehicle_weapon_imperator2_50cal_laser"), REVERSE_JOAAT("weapon_sawnoffshotgun"), REVERSE_JOAAT("weapon_smg_mk2"), REVERSE_JOAAT("vehicle_weapon_mine_spike_rc"), REVERSE_JOAAT("vehicle_weapon_akula_missile"), REVERSE_JOAAT("vehicle_weapon_mine_tar_rc"), REVERSE_JOAAT("weapon_bullpuprifle"), REVERSE_JOAAT("weapon_firework"), REVERSE_JOAAT("vehicle_weapon_turret_technical"), REVERSE_JOAAT("weapon_combatmg"), REVERSE_JOAAT("vehicle_weapon_trailer_dualaa"), REVERSE_JOAAT("vehicle_weapon_flamethrower_scifi"), REVERSE_JOAAT("vehicle_weapon_khanjali_cannon_heavy"), REVERSE_JOAAT("weapon_carbinerifle"), REVERSE_JOAAT("vehicle_weapon_mule4_mg"), REVERSE_JOAAT("weapon_crowbar"), REVERSE_JOAAT("weapon_bullpuprifle_mk2"), REVERSE_JOAAT("vehicle_weapon_mine_slick_rc"), REVERSE_JOAAT("vehicle_weapon_pounder2_mini"), REVERSE_JOAAT("vehicle_weapon_viseris_mg"), REVERSE_JOAAT("vehicle_weapon_akula_barrage"), REVERSE_JOAAT("weapon_snspistol_mk2"), REVERSE_JOAAT("weapon_briefcase"), REVERSE_JOAAT("weapon_bleeding"), REVERSE_JOAAT("weapon_flashlight"), REVERSE_JOAAT("vehicle_weapon_oppressor_missile"), REVERSE_JOAAT("vehicle_weapon_impaler3_50cal_laser"), REVERSE_JOAAT("vehicle_weapon_seasparrow2_minigun"), REVERSE_JOAAT("weapon_navyrevolver"), REVERSE_JOAAT("vehicle_weapon_pounder2_barrage"), REVERSE_JOAAT("weapon_dagger"), REVERSE_JOAAT("weapon_electric_fence"), REVERSE_JOAAT("vehicle_weapon_pounder2_gl"), REVERSE_JOAAT("weapon_grenade"), REVERSE_JOAAT("weapon_poolcue"), REVERSE_JOAAT("weapon_bat"), REVERSE_JOAAT("weapon_specialcarbine_mk2"), REVERSE_JOAAT("weapon_doubleaction"), REVERSE_JOAAT("vehicle_weapon_avenger_cannon"), REVERSE_JOAAT("weapon_pistol50"), REVERSE_JOAAT("weapon_knife"), REVERSE_JOAAT("vehicle_weapon_avenger3_minigun"), REVERSE_JOAAT("vehicle_weapon_bomb"), REVERSE_JOAAT("vehicle_weapon_deluxo_mg"), REVERSE_JOAAT("weapon_mg"), REVERSE_JOAAT("weapon_militaryrifle"), REVERSE_JOAAT("weapon_bullpupshotgun"), REVERSE_JOAAT("vehicle_weapon_tampa_missile"), REVERSE_JOAAT("vehicle_weapon_speedo4_turret_mini"), REVERSE_JOAAT("vehicle_weapon_player_hunter"), REVERSE_JOAAT("weapon_bzgas"), REVERSE_JOAAT("vehicle_weapon_dune_grenadelauncher"), REVERSE_JOAAT("vehicle_weapon_mortar_explosive"), REVERSE_JOAAT("vehicle_weapon_cherno_missile"), REVERSE_JOAAT("weapon_unarmed"), REVERSE_JOAAT("weapon_grenadelauncher"), REVERSE_JOAAT("weapon_run_over_by_car"), REVERSE_JOAAT("vehicle_weapon_barrage_rear_gl"), REVERSE_JOAAT("vehicle_weapon_turret_valkyrie"), REVERSE_JOAAT("vehicle_weapon_nightshark_mg"), REVERSE_JOAAT("weapon_rabbit"), REVERSE_JOAAT("weapon_musket"), REVERSE_JOAAT("vehicle_weapon_insurgent_minigun"), REVERSE_JOAAT("vehicle_weapon_sub_missile_homing"), REVERSE_JOAAT("weapon_proxmine"), REVERSE_JOAAT("weapon_advancedrifle"), REVERSE_JOAAT("weapon_raypistol"), REVERSE_JOAAT("vehicle_weapon_tula_dualmg"), REVERSE_JOAAT("vehicle_weapon_rotors"), REVERSE_JOAAT("weapon_rpg"), REVERSE_JOAAT("vehicle_weapon_flamethrower"), REVERSE_JOAAT("vehicle_weapon_turret_dinghy5_50cal"), REVERSE_JOAAT("vehicle_weapon_dominator5_50cal_laser"), REVERSE_JOAAT("weapon_hammerhead_shark"), REVERSE_JOAAT("vehicle_weapon_deluxo_missile"), REVERSE_JOAAT("vehicle_weapon_turret_boxville"), REVERSE_JOAAT("vehicle_weapon_akula_turret_single"), REVERSE_JOAAT("weapon_rayminigun"), REVERSE_JOAAT("vehicle_weapon_imperator_50cal"), REVERSE_JOAAT("vehicle_weapon_mogul_dualturret"), REVERSE_JOAAT("weapon_pipebomb"), REVERSE_JOAAT("weapon_hazardcan"), REVERSE_JOAAT("vehicle_weapon_raiju_cannons"), REVERSE_JOAAT("weapon_small_dog"), REVERSE_JOAAT("vehicle_weapon_scramjet_missile"), REVERSE_JOAAT("weapon_minismg"), REVERSE_JOAAT("vehicle_weapon_revolter_mg"), REVERSE_JOAAT("weapon_vehicle_rocket"), REVERSE_JOAAT("weapon_snspistol"), REVERSE_JOAAT("weapon_pistol_mk2"), REVERSE_JOAAT("weapon_assaultrifle"), REVERSE_JOAAT("weapon_specialcarbine"), REVERSE_JOAAT("weapon_revolver"), REVERSE_JOAAT("vehicle_weapon_ardent_mg"), REVERSE_JOAAT("vehicle_weapon_microlight_mg"), REVERSE_JOAAT("weapon_marksmanrifle"), REVERSE_JOAAT("vehicle_weapon_annihilator2_missile"), REVERSE_JOAAT("weapon_heavyrifle"), REVERSE_JOAAT("vehicle_weapon_speedo4_mg"), REVERSE_JOAAT("vehicle_weapon_dogfighter_missile"), REVERSE_JOAAT("vehicle_weapon_annihilator2_mini"), REVERSE_JOAAT("weapon_revolver_mk2"), REVERSE_JOAAT("weapon_hit_by_water_cannon"), REVERSE_JOAAT("weapon_battleaxe"), REVERSE_JOAAT("vehicle_weapon_searchlight"), REVERSE_JOAAT("weapon_fall"), REVERSE_JOAAT("vehicle_weapon_plane_rocket"), REVERSE_JOAAT("weapon_handcuffs"), REVERSE_JOAAT("vehicle_weapon_dune_mg"), REVERSE_JOAAT("weapon_tacticalrifle"), REVERSE_JOAAT("weapon_heavypistol"), REVERSE_JOAAT("vehicle_weapon_radar"), REVERSE_JOAAT("vehicle_weapon_subcar_missile"), REVERSE_JOAAT("vehicle_weapon_oppressor2_cannon"), REVERSE_JOAAT("vehicle_weapon_speedo4_turret_mg"), REVERSE_JOAAT("vehicle_weapon_bruiser_50cal"), REVERSE_JOAAT("vehicle_weapon_bombushka_cannon"), REVERSE_JOAAT("weapon_knuckle"), REVERSE_JOAAT("vehicle_weapon_oppressor_mg"), REVERSE_JOAAT("vehicle_weapon_mine_spike"), REVERSE_JOAAT("vehicle_weapon_tampa_fixedminigun"), REVERSE_JOAAT("weapon_machinepistol"), REVERSE_JOAAT("weapon_emplauncher"), REVERSE_JOAAT("vehicle_weapon_technical_minigun"), REVERSE_JOAAT("weapon_metaldetector"), REVERSE_JOAAT("weapon_combatmg_mk2"), REVERSE_JOAAT("weapon_marksmanpistol"), REVERSE_JOAAT("vehicle_weapon_mule4_turret_gl"), REVERSE_JOAAT("weapon_machete"), REVERSE_JOAAT("vehicle_weapon_turret_patrolboat_50cal"), REVERSE_JOAAT("weapon_fire"), REVERSE_JOAAT("vehicle_weapon_menacer_mg"), REVERSE_JOAAT("weapon_switchblade"), REVERSE_JOAAT("vehicle_weapon_scarab2_50cal_laser"), REVERSE_JOAAT("weapon_garbagebag"), REVERSE_JOAAT("vehicle_weapon_oppressor2_mg"), REVERSE_JOAAT("weapon_cat"), REVERSE_JOAAT("vehicle_weapon_player_lazer"), REVERSE_JOAAT("weapon_assaultshotgun"), REVERSE_JOAAT("vehicle_weapon_mogul_turret"), REVERSE_JOAAT("vehicle_weapon_mobileops_cannon"), REVERSE_JOAAT("vehicle_weapon_monster3_glkin"), REVERSE_JOAAT("weapon_animal_retriever"), REVERSE_JOAAT("vehicle_weapon_mogul_dualnose"), REVERSE_JOAAT("vehicle_weapon_rogue_cannon"), REVERSE_JOAAT("vehicle_weapon_subcar_torpedo"), REVERSE_JOAAT("vehicle_weapon_deathbike2_minigun_laser"), REVERSE_JOAAT("vehicle_weapon_comet_mg"), REVERSE_JOAAT("vehicle_weapon_granger2_mg"), REVERSE_JOAAT("vehicle_weapon_savestra_mg"), REVERSE_JOAAT("vehicle_weapon_brutus_50cal"), REVERSE_JOAAT("vehicle_weapon_cannon_blazer"), REVERSE_JOAAT("weapon_dbshotgun"), REVERSE_JOAAT("weapon_assaultsmg"), REVERSE_JOAAT("vehicle_weapon_player_laser"), REVERSE_JOAAT("vehicle_weapon_vigilante_mg"), REVERSE_JOAAT("vehicle_weapon_mine_tar"), REVERSE_JOAAT("weapon_deer"), REVERSE_JOAAT("vehicle_weapon_rctank_flame"), REVERSE_JOAAT("vehicle_weapon_mogul_nose"), REVERSE_JOAAT("vehicle_weapon_barrage_top_mg"), REVERSE_JOAAT("weapon_acidpackage"), REVERSE_JOAAT("vehicle_weapon_dominator4_50cal"), REVERSE_JOAAT("vehicle_weapon_space_rocket"), REVERSE_JOAAT("weapon_hatchet"), REVERSE_JOAAT("weapon_bottle"), REVERSE_JOAAT("weapon_animal"), REVERSE_JOAAT("weapon_killer_whale"), REVERSE_JOAAT("weapon_carbinerifle_mk2"), REVERSE_JOAAT("weapon_digiscanner"), REVERSE_JOAAT("weapon_smokegrenade"), REVERSE_JOAAT("weapon_railgunxm3"), REVERSE_JOAAT("vehicle_weapon_avenger3_missile"), REVERSE_JOAAT("weapon_drowning"),
		REVERSE_JOAAT("WEAPON_STUNROD"),
		// gadgets
		REVERSE_JOAAT("GADGET_NIGHTVISION"),
		REVERSE_JOAAT("GADGET_PARACHUTE"),
		// peds_crash
		REVERSE_JOAAT("slod_small_quadped"),
		REVERSE_JOAAT("slod_human"),
		REVERSE_JOAAT("slod_large_quadped"),
	};

	static void addToJoaatMap(const char* name)
	{
		joaat_map.emplace(rage::atStringHash(name), name);
	}

	void joaatToStringInit()
	{
		for (PedModel& ped : PedModel::all)
		{
			addToJoaatMap(ped.hash.str);

			std::string model_name = ped.hash.str;
			StringUtils::replace_all(model_name, "_", "");
			ped.command_names.emplace_back(utf8ToCmdName(std::move(model_name)));

			ped.hash.convertToHash();
		}
		for (const VehicleItem& item : vehicle_items)
		{
			addToJoaatMap(item.name);
		}
		for (const auto& object : g_objects)
		{
			addToJoaatMap(object);
		}
		for (const auto& object : g_objects_downtown)
		{
			addToJoaatMap(object);
		}
		for (auto& wc : g_weapon_components)
		{
			addToJoaatMap(wc.hash.str);
			wc.hash.convertToHash();
		}
		for (auto& wl : g_weapon_liveries)
		{
			for (auto& wc : wl.second)
			{
				addToJoaatMap(wc.hash.str);
				wc.hash.convertToHash();
			}
		}
	}

	void joaatToStringDeinit()
	{
		joaat_map.clear();
	}


	const char* joaatToStringRaw(const hash_t hash)
	{
		auto i = joaat_map.find(hash);
		if (i != joaat_map.end())
		{
			return i->second;
		}
		return nullptr;
	}

	std::string joaatToString(const hash_t hash)
	{
		const char* const name = joaatToStringRaw(hash);
		if (name != nullptr)
		{
			return name;
		}
		return Util::to_padded_hex_string_with_0x(hash);
	}

	hash_t stringToJoaat(const std::string& str)
	{
		if (str.substr(0, 2) == "0x")
		{
			return std::stoul(str.c_str() + 2, nullptr, 16);
		}
		return rage::atStringHash(str);
	}
}
