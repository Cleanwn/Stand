#pragma once

#include <vector>

#include "ToggleableBehaviourIpl.hpp"

namespace Stand
{
	enum IplSpecialBehaviour : uint8_t
	{
		IPLSPEC_NONE,
		IPLSPEC_MP_WORLD_STATE,
		IPLSPEC_SET_UNBLOCK_GLOBAL,
	};

	struct IplItem
	{
		const Label menu_name;
		const std::vector<CommandName> command_names;
		const v3 coords;
		ToggleableBehaviourIpl* const togglable_behaviour = nullptr;
		const IplData data{};
		const IplSpecialBehaviour special_behaviour = IPLSPEC_NONE;

		constexpr IplItem(Label&& menu_name, std::vector<CommandName>&& command_names, v3&& coords, std::vector<const char*>&& request, std::vector<const char*>&& remove = {}, std::vector<const char*>&& remove_enable_only = {}, const IplSpecialBehaviour special_behaviour = IPLSPEC_NONE) noexcept
			: menu_name(std::move(menu_name)), command_names(std::move(command_names)), coords(std::move(coords)), data{ std::move(request), std::move(remove), std::move(remove_enable_only), {} }, special_behaviour(special_behaviour)
		{
		}

		constexpr IplItem(Label&& menu_name, std::vector<CommandName>&& command_names, v3&& coords, std::vector<const char*>&& request, std::vector<const char*>&& remove, std::vector<const char*>&& remove_enable_only, std::vector<DoorData>&& doors, const IplSpecialBehaviour special_behaviour = IPLSPEC_NONE) noexcept
			: menu_name(std::move(menu_name)), command_names(std::move(command_names)), coords(std::move(coords)), data{ std::move(request), std::move(remove), std::move(remove_enable_only), std::move(doors) }, special_behaviour(special_behaviour)
		{
		}

		constexpr IplItem(Label&& menu_name, std::vector<CommandName>&& command_names, v3&& coords, ToggleableBehaviourIpl* const togglable_behaviour, const IplSpecialBehaviour special_behaviour = IPLSPEC_NONE) noexcept
			: menu_name(std::move(menu_name)), command_names(std::move(command_names)), coords(std::move(coords)), togglable_behaviour(togglable_behaviour), special_behaviour(special_behaviour)
		{
		}

		const IplData* getData() const noexcept
		{
			if (togglable_behaviour != nullptr)
			{
				return &togglable_behaviour->data;
			}
			return &data;
		}
	};

	inline IplItem ipls[] = {
		{LOC("CAYO"), CMDNAMES("iplcayoperico", "iplperico", "iplcayopercio", "iplpercio"), v3(4906.2554f, -4912.7646f, 3.3630776f), {"h4_islandx_terrain_01_lod","h4_islandx_terrain_01_slod","h4_islandx_terrain_02","h4_islandx_terrain_02_lod","h4_islandx_terrain_02_slod","h4_islandx_terrain_03","h4_islandx_terrain_03_lod","h4_islandx_terrain_04","h4_islandx_terrain_04_lod","h4_islandx_terrain_04_slod","h4_islandx_terrain_05","h4_islandx_terrain_05_lod","h4_islandx_terrain_05_slod","h4_islandx_terrain_06","h4_mph4_terrain_01_grass_0","h4_mph4_terrain_01_grass_1","h4_mph4_terrain_02_grass_0","h4_mph4_terrain_02_grass_1","h4_mph4_terrain_02_grass_2","h4_mph4_terrain_02_grass_3","h4_mph4_terrain_04_grass_0","h4_mph4_terrain_04_grass_1","h4_mph4_terrain_05_grass_0","h4_mph4_terrain_06_grass_0","h4_islandx_terrain_01","h4_islandx_terrain_06_lod","h4_islandx_terrain_06_slod","h4_islandx_terrain_props_05_a","h4_islandx_terrain_props_05_a_lod","h4_islandx_terrain_props_05_b","h4_islandx_terrain_props_05_b_lod","h4_islandx_terrain_props_05_c","h4_islandx_terrain_props_05_c_lod","h4_islandx_terrain_props_05_d","h4_islandx_terrain_props_05_d_lod","h4_islandx_terrain_props_05_d_slod","h4_islandx_terrain_props_05_e","h4_islandx_terrain_props_05_e_lod","h4_islandx_terrain_props_05_e_slod","h4_islandx_terrain_props_05_f","h4_islandx_terrain_props_05_f_lod","h4_islandx_terrain_props_05_f_slod","h4_islandx_terrain_props_06_a","h4_islandx_terrain_props_06_a_lod","h4_islandx_terrain_props_06_a_slod","h4_islandx_terrain_props_06_b","h4_islandx_terrain_props_06_b_lod","h4_islandx_terrain_props_06_b_slod","h4_islandx_terrain_props_06_c","h4_islandx_terrain_props_06_c_lod","h4_islandx_terrain_props_06_c_slod","h4_mph4_terrain_01","h4_mph4_terrain_01_long_0","h4_mph4_terrain_02","h4_mph4_terrain_03","h4_mph4_terrain_04","h4_mph4_terrain_05","h4_mph4_terrain_06","h4_mph4_terrain_06_strm_0","h4_mph4_terrain_lod","h4_mph4_terrain_occ_00","h4_mph4_terrain_occ_01","h4_mph4_terrain_occ_02","h4_mph4_terrain_occ_03","h4_mph4_terrain_occ_04","h4_mph4_terrain_occ_05","h4_mph4_terrain_occ_06","h4_mph4_terrain_occ_07","h4_mph4_terrain_occ_08","h4_mph4_terrain_occ_09","h4_boatblockers","h4_islandx","h4_islandx_disc_strandedshark","h4_islandx_disc_strandedshark_lod","h4_islandx_disc_strandedwhale","h4_islandx_disc_strandedwhale_lod","h4_islandx_props","h4_islandx_props_lod","h4_islandx_sea_mines","h4_mph4_island","h4_mph4_island_long_0","h4_mph4_island_strm_0","h4_aa_guns","h4_aa_guns_lod","h4_beach","h4_beach_bar_props","h4_beach_lod","h4_beach_party","h4_beach_party_lod","h4_beach_props","h4_beach_props_lod","h4_beach_props_party","h4_beach_props_slod","h4_beach_slod","h4_islandairstrip","h4_islandairstrip_doorsclosed","h4_islandairstrip_doorsclosed_lod","h4_islandairstrip_doorsopen","h4_islandairstrip_doorsopen_lod","h4_islandairstrip_hangar_props","h4_islandairstrip_hangar_props_lod","h4_islandairstrip_hangar_props_slod","h4_islandairstrip_lod","h4_islandairstrip_props","h4_islandairstrip_propsb","h4_islandairstrip_propsb_lod","h4_islandairstrip_propsb_slod","h4_islandairstrip_props_lod","h4_islandairstrip_props_slod","h4_islandairstrip_slod","h4_islandxcanal_props","h4_islandxcanal_props_lod","h4_islandxcanal_props_slod","h4_islandxdock","h4_islandxdock_lod","h4_islandxdock_props","h4_islandxdock_props_2","h4_islandxdock_props_2_lod","h4_islandxdock_props_2_slod","h4_islandxdock_props_lod","h4_islandxdock_props_slod","h4_islandxdock_slod","h4_islandxdock_water_hatch","h4_islandxtower","h4_islandxtower_lod","h4_islandxtower_slod","h4_islandxtower_veg","h4_islandxtower_veg_lod","h4_islandxtower_veg_slod","h4_islandx_barrack_hatch","h4_islandx_barrack_props","h4_islandx_barrack_props_lod","h4_islandx_barrack_props_slod","h4_islandx_checkpoint","h4_islandx_checkpoint_lod","h4_islandx_checkpoint_props","h4_islandx_checkpoint_props_lod","h4_islandx_checkpoint_props_slod","h4_islandx_maindock","h4_islandx_maindock_lod","h4_islandx_maindock_props","h4_islandx_maindock_props_2","h4_islandx_maindock_props_2_lod","h4_islandx_maindock_props_2_slod","h4_islandx_maindock_props_lod","h4_islandx_maindock_props_slod","h4_islandx_maindock_slod","h4_islandx_mansion","h4_islandx_mansion_b","h4_islandx_mansion_b_lod","h4_islandx_mansion_b_side_fence","h4_islandx_mansion_b_slod","h4_islandx_mansion_entrance_fence","h4_islandx_mansion_guardfence","h4_islandx_mansion_lights","h4_islandx_mansion_lockup_01","h4_islandx_mansion_lockup_01_lod","h4_islandx_mansion_lockup_02","h4_islandx_mansion_lockup_02_lod","h4_islandx_mansion_lockup_03","h4_islandx_mansion_lockup_03_lod","h4_islandx_mansion_lod","h4_islandx_mansion_office","h4_islandx_mansion_office_lod","h4_islandx_mansion_props","h4_islandx_mansion_props_lod","h4_islandx_mansion_props_slod","h4_islandx_mansion_slod","h4_islandx_mansion_vault","h4_islandx_mansion_vault_lod","h4_island_padlock_props","h4_mansion_gate_broken","h4_mansion_gate_closed","h4_mansion_remains_cage","h4_mph4_airstrip","h4_mph4_airstrip_interior_0_airstrip_hanger","h4_mph4_beach","h4_mph4_dock","h4_mph4_island_lod","h4_mph4_island_ne_placement","h4_mph4_island_nw_placement","h4_mph4_island_se_placement","h4_mph4_island_sw_placement","h4_mph4_mansion","h4_mph4_mansion_b","h4_mph4_mansion_b_strm_0","h4_mph4_mansion_strm_0","h4_mph4_wtowers","h4_ne_ipl_00","h4_ne_ipl_00_lod","h4_ne_ipl_00_slod","h4_ne_ipl_01","h4_ne_ipl_01_lod","h4_ne_ipl_01_slod","h4_ne_ipl_02","h4_ne_ipl_02_lod","h4_ne_ipl_02_slod","h4_ne_ipl_03","h4_ne_ipl_03_lod","h4_ne_ipl_03_slod","h4_ne_ipl_04","h4_ne_ipl_04_lod","h4_ne_ipl_04_slod","h4_ne_ipl_05","h4_ne_ipl_05_lod","h4_ne_ipl_05_slod","h4_ne_ipl_06","h4_ne_ipl_06_lod","h4_ne_ipl_06_slod","h4_ne_ipl_07","h4_ne_ipl_07_lod","h4_ne_ipl_07_slod","h4_ne_ipl_08","h4_ne_ipl_08_lod","h4_ne_ipl_08_slod","h4_ne_ipl_09","h4_ne_ipl_09_lod","h4_ne_ipl_09_slod","h4_nw_ipl_00","h4_nw_ipl_00_lod","h4_nw_ipl_00_slod","h4_nw_ipl_01","h4_nw_ipl_01_lod","h4_nw_ipl_01_slod","h4_nw_ipl_02","h4_nw_ipl_02_lod","h4_nw_ipl_02_slod","h4_nw_ipl_03","h4_nw_ipl_03_lod","h4_nw_ipl_03_slod","h4_nw_ipl_04","h4_nw_ipl_04_lod","h4_nw_ipl_04_slod","h4_nw_ipl_05","h4_nw_ipl_05_lod","h4_nw_ipl_05_slod","h4_nw_ipl_06","h4_nw_ipl_06_lod","h4_nw_ipl_06_slod","h4_nw_ipl_07","h4_nw_ipl_07_lod","h4_nw_ipl_07_slod","h4_nw_ipl_08","h4_nw_ipl_08_lod","h4_nw_ipl_08_slod","h4_nw_ipl_09","h4_nw_ipl_09_lod","h4_nw_ipl_09_slod","h4_se_ipl_00","h4_se_ipl_00_lod","h4_se_ipl_00_slod","h4_se_ipl_01","h4_se_ipl_01_lod","h4_se_ipl_01_slod","h4_se_ipl_02","h4_se_ipl_02_lod","h4_se_ipl_02_slod","h4_se_ipl_03","h4_se_ipl_03_lod","h4_se_ipl_03_slod","h4_se_ipl_04","h4_se_ipl_04_lod","h4_se_ipl_04_slod","h4_se_ipl_05","h4_se_ipl_05_lod","h4_se_ipl_05_slod","h4_se_ipl_06","h4_se_ipl_06_lod","h4_se_ipl_06_slod","h4_se_ipl_07","h4_se_ipl_07_lod","h4_se_ipl_07_slod","h4_se_ipl_08","h4_se_ipl_08_lod","h4_se_ipl_08_slod","h4_se_ipl_09","h4_se_ipl_09_lod","h4_se_ipl_09_slod","h4_sw_ipl_00","h4_sw_ipl_00_lod","h4_sw_ipl_00_slod","h4_sw_ipl_01","h4_sw_ipl_01_lod","h4_sw_ipl_01_slod","h4_sw_ipl_02","h4_sw_ipl_02_lod","h4_sw_ipl_02_slod","h4_sw_ipl_03","h4_sw_ipl_03_lod","h4_sw_ipl_03_slod","h4_sw_ipl_04","h4_sw_ipl_04_lod","h4_sw_ipl_04_slod","h4_sw_ipl_05","h4_sw_ipl_05_lod","h4_sw_ipl_05_slod","h4_sw_ipl_06","h4_sw_ipl_06_lod","h4_sw_ipl_06_slod","h4_sw_ipl_07","h4_sw_ipl_07_lod","h4_sw_ipl_07_slod","h4_sw_ipl_08","h4_sw_ipl_08_lod","h4_sw_ipl_08_slod","h4_sw_ipl_09","h4_sw_ipl_09_lod","h4_sw_ipl_09_slod","h4_underwater_gate_closed","h4_islandx_placement_01","h4_islandx_placement_02","h4_islandx_placement_03","h4_islandx_placement_04","h4_islandx_placement_05","h4_islandx_placement_06","h4_islandx_placement_07","h4_islandx_placement_08","h4_islandx_placement_09","h4_islandx_placement_10","h4_mph4_island_placement"}, {}, {}, IPLSPEC_MP_WORLD_STATE},
		{LOC("YANK"), CMDNAMES("yank", "northyankton"), v3(3359.8732910156f, -4849.5229492188f, 111.67414855957f), {"plg_01","prologue01","prologue01c","prologue01d","prologue01e","prologue01f","prologue01g","prologue01h","prologue01i","prologue01j","prologue01k","prologue01z","plg_02","prologue02","plg_03","prologue03","prologue03_grv_cov","prologue03b","prologue_grv_torch","des_protree_end","des_protree_start","plg_04","prologue04","prologue04_cover","prologue04b","plg_05","prologue05","prologue05b","plg_06","prologue06","prologue06_int","prologue06_panne","prologue06b","prologue_m2_door","plg_rd","prologuerd","prologuerdb","prologue_DistantLights","prologue_LODLights"}},
		{LOC("IPL_CLKNBLLFCTRY"), CMDNAMES("cluckingbellfactory"), v3(-146.3837f, 6161.5f, 30.2062f), {"CS1_02_cf_onmission1","CS1_02_cf_onmission2","CS1_02_cf_onmission3","CS1_02_cf_onmission4"}, {"CS1_02_cf_offmission"}, {}, {{-1331552374, ATSTRINGHASH("prop_fnclink_03gate5"), -182.91f, 6168.37f, 32.14f}}},
		{LOC("IPL_USSLUX"), CMDNAMES("ussluxington"), v3(3073.520264f, -4715.202148f, 16.089409f), {"hei_carrier","hei_carrier_DistantLights","hei_Carrier_int1","hei_Carrier_int2","hei_Carrier_int3","hei_Carrier_int4","hei_Carrier_int5","hei_Carrier_int6","hei_carrier_LODLights"}, {}, {}, IPLSPEC_SET_UNBLOCK_GLOBAL},
		{LOC("IPL_MORGUE"), CMDNAMES("morgue"), v3(244.9f, -1374.7f, 39.5f), &g_ipl_morgue},
		{LOC("IPL_HOSPITAL"), CMDNAMES("hospital"), v3(356.8f, -590.1f, 43.3f), &g_ipl_hospital},
		{LOC("IPL_AYACHT"), CMDNAMES("seriesayacht"), v3(-2045.8f, -1031.2f, 11.9f), {"hei_yacht_heist","hei_yacht_heist_Bar","hei_yacht_heist_Bedrm","hei_yacht_heist_Bridge","hei_yacht_heist_DistantLights","hei_yacht_heist_enginrm","hei_yacht_heist_LODLights","hei_yacht_heist_Lounge"}, {}, {}, IPLSPEC_MP_WORLD_STATE},
		{LOC("IPL_RANCH"), CMDNAMES("ranch"), v3(2441.2f, 4968.5f, 51.7f), {"farm", "farmint", "farm_props", "des_farmhouse"}, {"farm_burnt", "farm_burnt_props"}, {"farmint_cap"}},
		{LOC("IPL_RANCH_LIT"), CMDNAMES("ranchfire"), v3(2447.9f, 4973.4f, 47.7f), {"des_farmhs_endimap","des_farmhs_end_occ","des_farmhs_startimap","des_farmhs_start_occ"}},
		{LOC("IPL_LOSTHOUSE"), CMDNAMES("lostmcclubhouse"), v3(984.1552f, -95.3662f, 74.50f), &g_ipl_lost_mc_clubhouse, IPLSPEC_MP_WORLD_STATE},
		{LOC("IPL_LSTRFCTRY"), CMDNAMES("lestersfactory"), v3(716.84f, -962.05f, 31.59f), {"id2_14_during_door", "id2_14_during1"}, {"id2_14_post_no_int"}, {"id2_14_pre_no_int"}},
		{LOC("FMEORSHME"), CMDNAMES("fameorshame"), v3(-248.49159240722656f, -2010.509033203125f, 34.57429885864258f), &g_ipl_fameorshame},
		{LOC("FCEBK"), CMDNAMES("lifeinvader"), v3(-1047.9f, -233.0f, 39.0f), &g_ipl_lifeinvader},
		{LOC("IPL_JEWEL"), CMDNAMES("vangelicofinejewelry"), v3(-630.4f, -236.7f, 40.0f), &g_ipl_jewel},
		{LOC("IPL_MXRD"), CMDNAMES("maxrenda"), v3(-585.8247f, -282.72f, 35.45475f), &g_ipl_max_renda},
		{LOC("IPL_TRNWRCK"), CMDNAMES("trainwreck"), v3(-532.1309f, 4526.187f, 88.7955f), {"canyonriver01_traincrash", "railing_end"}, {"canyonriver01", "railing_start"}},
		{LOC("IPL_UFOEYE"), CMDNAMES("ufoeye", "eyeufo"), v3(496.58383, 5608.7427, 795.7164), {"ufo_eye"}},
		{LOC("IPL_BCHOUSE"), CMDNAMES("banhamcanyonhouse"), v3(-3096.308838f, 346.084503f, 10.804105f), {"ch1_02_open"}},
		{LOC("IPL_REDCRPT"), CMDNAMES("redcarpet"), v3(295.178833f, 177.504059f, 103.622993f), {"redCarpet"}},
		{LOC("IPL_FIBLBBY"), CMDNAMES("fiblobby"), v3(110.4f, -744.2f, 45.7f), &g_ipl_fib_lobby},
		{LOC("IPL_FIBHELI"), CMDNAMES("fibhelicoptercrash"), v3(169.0f, -670.3f, 41.9f), {"DT1_05_HC_REQ"}, {"DT1_05_HC_REMOVE"}},
		{LOC("IPL_FIBRBBL"), CMDNAMES("fibrubble"), v3(74.29f, -736.05f, 46.76f), {"DT1_05_rubble"}},
		{LOC("IPL_FINBANK"), CMDNAMES("uniondepository"), v3(2.69689322f, -667.0166f, 16.1306286f), &g_ipl_union_dep},
		{LOC("IPL_PLNCRSH"), CMDNAMES("planecrashtrench"), v3(2803.5776f, 4752.6777f, 46.337643f), {"Plane_crash_trench"}},
		{LOC("IPL_TRIA2"), CMDNAMES("alamoseatriathlon"), v3(2384.969f, 4277.583f, 30.379f), {"CS2_06_TriAf02"}},
		{LOC("IPL_TIRA1"), CMDNAMES("lsiatriathlon"), v3(-1277.629f, -2030.913f, 1.2823f), {"AP1_04_TriAf01"}},
		{LOC("IPL_ZGATES"), CMDNAMES("zancudogates", "fortzancudogates"), v3(-1601.424f, 2808.213f, 16.2598f), {"cs3_07_mpgates"}},
		{LOC("IPL_BOXES"), CMDNAMES("sandyshoresairfieldboxes"), v3(1743.682f, 3286.251f, 40.0875f), {"airfield"}},
		{LOC("IPL_SPNN"), CMDNAMES("carwashspinners"), v3(55.7f, -1391.3f, 30.5f), {"Carwash_with_spinners"}},
		{LOC("IPL_MAZEGRFF"), CMDNAMES("mazebankbillboardgraffiti"), v3(2697.32f, 3162.18f, 58.1f), {"CS5_04_MazeBillboardGraffiti"}},
		{LOC("IPL_OILGRFF"), CMDNAMES("ronoilbillboardgraffiti"), v3(2101.490234f, 3071.507568f, 46.552769f), {"CS5_Roads_RonOilGraffiti"}},
		{LOC("IPL_FRUITBB"), CMDNAMES("ifruitbillboard"), v3(-1327.46f, -274.82f, 54.25f), {"FruitBB"}},
		{LOC("IPL_MBILL1"), CMDNAMES("meltdownbillboard1"), v3(-351.0f, -1324.0f, 44.02f), {"sc1_01_newbil"}, {"SC1_01_OldBil"}},
		{LOC("IPL_MBILL2"), CMDNAMES("meltdownbillboard2"), v3(391.81f, -962.71f, 41.97f), {"dt1_17_newbil"}, {"DT1_17_OldBil"}},
		{LOC("IPL_GRVE"), CMDNAMES("hillvalleychurchgrave"), v3(-282.4638f, 2835.845f, 55.91446f), {"lr_cs6_08_grave_open"}, {"lr_cs6_08_grave_closed"}, {}, IPLSPEC_MP_WORLD_STATE},
	};
}
