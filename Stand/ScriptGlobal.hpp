#pragma once

// shop_controller

#define GLOBAL_UNK_GAME_STATE_XMAS									32949

// am_contact_requests

#define GLOBAL_CONTACT_TIMEOUT										2685444 + 4342 + 245
#define GLOBAL_CONTACT_TIMEOUT_SIZE									2
#define GLOBAL_CONTACT_TIMEOUT_MECHANIC								7

// freemode

#pragma region Broadcast Data
#define GPBD_FM_2 1882632
#define GPBD_Interactions 4535209
#define GPBD_FM 1845281
#define GPBD_MissionName 1873538
#define GPBD_Kicking 1874051
#define GPBD_SCTV 1921352
#define GPBD_FM_HeistPlanning 1877285
#define g_AMC_playerBD 2699358
#define GPBD_FM_3 1887305
#define GlobalplayerBD 2657971
#define GPBD_FM_4 1906887
#define GSBD 2648938
#define GSBD_BlockB 2650436
#define GSBD_BlockC 2652592
#define GSBD_MissionsShared 2625825
#define GSBD_MissionRequest 2655268
#define GSBD_MissionList 2655925
#define GSBD_ExclusionAreas 2656135
#define GSBD_ActivitySelector 2656360
#define GSBD_Betting 2657219
#define GSBD_HoldUp 2657800
#define GSBD_SyncedInteractions 4535818
#define GSBD_FM 1835505
#define GSBD_Kicking 1877252
#define GSBD_HeistPlanning 1928958
#define GSBD_PropertyInstances 1936863
#define GSBD_FM_events 1916617
#define GPBD_CasinoHeist 1969212
#define GSBD_RandomEvents 1882247
#define GPBD_HeistIsland 1973625
#define GPBD_NetHeistPlanningGeneric 1972760
#define GPBD_Flow 1975605
#pragma endregion Broadcast Data

#define GlobalServerBD_FM GSBD_FM

#define GLOBAL_ATOMIZER_XMAS_TINT									104497			// (joaat("component_raypistol_varmod_xmas18"), joaat("weapon_raypistol"), -1)
#define GLOBAL_PRIVATE_SESSION										g_sTransitionSessionData + 746	// NETWORK::NETWORK_SESSION_GET_PRIVATE_SLOTS() > 0
#define GLOBAL_TRANSITION_STATE										1575011			// NETWORK::NETWORK_TRANSITION_ADD_STAGE(iVar2
#define GLOBAL_DISABLE_DRIPFEED										2707348

#define GLOBAL_NEW_SESSION_UNK1										GLOBAL_UNK_GAME_STATE_XMAS
#define GLOBAL_NEW_SESSION_UNK2										1574629
#define GLOBAL_NEW_SESSION_UNK3										1575030
#define GLOBAL_NEW_SESSION_SCTV										2697732

#define GLOBAL_PLAYER_STAT_UNKBETTINGIDX							97 + 28
#define GLOBAL_PLAYER_STAT_CONTROL_TYPE								97 + 29
#define GLOBAL_PLAYER_STAT_GANGCALLOWNER							/* iGangCallOwnerBitset */ 141
#define GLOBAL_PLAYER_STAT_GANGCALLTARGET							/* iGangCallTargetID */ 142
#define GLOBAL_PLAYER_STAT_GANGCALLSEENRUNNING						/* iGangCallSeenRunning */ 143
#define GLOBAL_PLAYER_STAT_RP										206 + 1
#define GLOBAL_PLAYER_STAT_RANK										206 + 6
#define GLOBAL_PLAYER_STAT_WALLET									206 + 3
#define GLOBAL_PLAYER_STAT_KD										206 + 26
#define GLOBAL_PLAYER_STAT_KILLS									206 + 28
#define GLOBAL_PLAYER_STAT_DEATHS									206 + 29
#define GLOBAL_PLAYER_STAT_FAVOURITE_STATION						206 + 53 // mpply_most_favorite_station
#define GLOBAL_PLAYER_STAT_PROSTITUTES_FREQUENTED					206 + 54
#define GLOBAL_PLAYER_STAT_LAP_DANCES_BOUGHT						206 + 55
#define GLOBAL_PLAYER_STAT_MONEY									206 + 56
#define GLOBAL_PLAYER_STAT_FAVOURITE_VEHICLE						206 + 58
#define GLOBAL_PLAYER_STAT_FAVOURITE_WEAPON							206 + 59
#define GLOBAL_PLAYER_STAT_APTID									268 + 35
#define GLOBAL_PLAYER_STAT_BUNKER_ID_BASE							268 + 197 // [5 /*13*/]
#define GLOBAL_PLAYER_STAT_BUNKER_ID_SIZE							13
#define GLOBAL_PLAYER_STAT_BUNKER_ID_OFFSET							5
#define GLOBAL_PLAYER_STAT_FACILITY_ID								268 + 304 // 0 if none owned
#define GLOBAL_PLAYER_STAT_NIGHTCLUB_ID								268 + 358

#define GLOBAL_PLAYER_STAT_VEHBITSET1								268 + 370		// == 646)
#define GLOBAL_PLAYER_STAT_VEHBITSET1_RCBANDITO						29

#define GLOBAL_PLAYER_STAT_VEHBITSET2								268 + 433 + 2	// == 742)
#define GLOBAL_PLAYER_STAT_VEHBITSET2_MINITANK						16

#define GLOBAL_PLAYER_STAT_VEHBITSET3								268 + 433 + 3
#define GLOBAL_PLAYER_STAT_VEHBITSET3_RCPV							6

#define g_events_sGBWorkData										11
#define	g_events_sGBWorkData_iModelHideBitset						379
#define eMH_MODEL_HIDE_BITSET_CrashSite2_activate_weather			9

#define GLOBAL_PLAYERSTATUS2_netID_PV								38		// VEHICLE::GET_VEHICLE_ATTACHED_TO_CARGOBOB(iVar7) != 0)
#define GLOBAL_PLAYERSTATUS2_BLIP									74		// == 122 || Global_
#define GLOBAL_PLAYERSTATUS2_OTRBITSET								GLOBAL_PLAYERSTATUS2_BLIP + 2
#define GLOBAL_PLAYERSTATUS2_BLIPVEHTYPE							GLOBAL_PLAYERSTATUS2_BLIP + 3
#define GLOBAL_PLAYERSTATUS2_OTR1									/* bOffTheRadar */ 211
#define GLOBAL_PLAYERSTATUS2_interiorCurrent						246		// INTERIOR::GET_INTERIOR_FROM_ENTITY(PLAYER::PLAYER_PED_ID()) != Global_
#define GLOBAL_PLAYERSTATUS2_iCurrentShop							247
#define GLOBAL_PLAYERSTATUS2_iCurrentHoldUp							248
#define GLOBAL_PLAYERSTATUS2_MENTALSTATE							269		// (133, fVar1, -1, 1);
#define GLOBAL_PLAYERSTATUS2_SPECTATING								270		// mpply_can_spectate
#define GLOBAL_PLAYERSTATUS2_INT_ID									322 + 8
#define GLOBAL_PLAYERSTATUS2_INT_INST								322 + 9
#define GLOBAL_PLAYERSTATUS2_BITSET1								426		// ORB_REMOT2
#define GLOBAL_PLAYERSTATUS2_BITSET1_ORBITALKIL						0
#define GLOBAL_PLAYERSTATUS2_CAYOPERICOFLAGS						444
#define GLOBAL_PLAYERSTATUS2_IMANI_OUTOFSIGHT_FLAGS					458
#define GLOBAL_PLAYERSTATUS2_IMANI_OUTOFSIGHT_POS					459

#define GLOBAL_ORG_BOSS												0
#define GLOBAL_ORG_FLAGS											4
#define GLOBAL_ORG_MEMBERS											11
#define GLOBAL_ORG_NEXT_BOSS										26
#define GLOBAL_ORG_MISSION											32
#define GLOBAL_ORG_bossLimo											75
#define GLOBAL_ORG_NAME												105
#define GLOBAL_ORG_LANG												121
#define GLOBAL_ORG_BANNED_SINCE										181
#define GLOBAL_ORG_MHBITSET /* iModelHideBitset */					205
#define GLOBAL_ORG_NAME_CEO_COPY									340
#define GLOBAL_ORG_NAME_MC_COPY										356
#define GLOBAL_ORG_TYPE												430 // -1 = none, 0 = org, 1 = mc // "Player_Goon"
#define GLOBAL_ORG_TYPE_COPY										467

#define GLOBAL_ORG_FLAGS_INVITEACCEPTED								7
#define GLOBAL_ORG_FLAGS_NOVOTEKICK3								9
#define GLOBAL_ORG_FLAGS_NOVOTEKICK4								20

#define GLOBAL_CEO_COLOUR_TO_SLOT									GLOBAL_CEO_COLOURS_BASE + 0
#define GLOBAL_CEO_COLOUR_BOSS_TO_SLOT								GLOBAL_CEO_COLOURS_BASE + 11
#define GLOBAL_CEO_COLOUR_SLOTS										GLOBAL_CEO_COLOURS_BASE + 44
#define GLOBAL_CEO_COLOUR_SLOTS_SIZE								2
#define GLOBAL_CEO_COLOUR_SLOT_BOSS									0
#define GLOBAL_CEO_COLOUR_SLOT_COLOUR								1

#define GLOBAL_PLAYERSTATUS1_MPINFOSTATE							68
#define GLOBAL_PLAYERSTATUS1_TAG									241 + 136
#define GLOBAL_PLAYERSTATUS1_TAG_TYPING								16

#define GLOBAL_LEAVE_ORG											MPGlobalsAmbience + 5249 + 226 // "GBTER_BIG", global being set to 6

#define GLOBAL_RESPAWN_BITSET										/* GLOBAL_RESPAWN_DATA + */ 756 // SET_STREAMED_TEXTURE_DICT_AS_NO_LONGER_NEEDED("TimerBars");

#define GLOBAL_PVS_PLATE_STYLE										0
#define GLOBAL_PVS_PLATE_TEXT										1
#define GLOBAL_PVS_PRIMARY_COLOUR									5
#define GLOBAL_PVS_SECONDARY_COLOUR									6
#define GLOBAL_PVS_PEARL_COLOUR										7
#define GLOBAL_PVS_WHEEL_COLOUR										8
#define GLOBAL_PVS_MODS_ARR											9
#define GLOBAL_PVS_MODS_VAR_ARR										59
#define GLOBAL_PVS_MODS_VAR_CUSTOM_FRONT_TYRES						0
#define GLOBAL_PVS_MODS_VAR_CUSTOM_BACK_TYRES						1
#define GLOBAL_PVS_TYRE_SMOKE_R										62
#define GLOBAL_PVS_TYRE_SMOKE_G										63
#define GLOBAL_PVS_TYRE_SMOKE_B										64
#define GLOBAL_PVS_WINDOW_TINT										65
#define GLOBAL_PVS_MODEL											66
#define GLOBAL_PVS_WHEEL_TYPE										69
#define GLOBAL_PVS_CUSTOM_R											71
#define GLOBAL_PVS_CUSTOM_G											72
#define GLOBAL_PVS_CUSTOM_B											73
#define GLOBAL_PVS_NEON_R											74
#define GLOBAL_PVS_NEON_G											75
#define GLOBAL_PVS_NEON_B											76
#define GLOBAL_PVS_FLAGS											77

#define GLOBAL_PVS_FLAGS_CUSTOM_PRIMARY_COLOUR						13
#define GLOBAL_PVS_FLAGS_CUSTOM_SECONDARY_COLOUR					12
#define GLOBAL_PVS_FLAGS_NEON_FRONT									28
#define GLOBAL_PVS_FLAGS_NEON_BACK									29
#define GLOBAL_PVS_FLAGS_NEON_LEFT									30
#define GLOBAL_PVS_FLAGS_NEON_RIGHT									31

#define GLOBAL_PVS_ENVEFF_SCALE										80
#define GLOBAL_PVS_INTERIOR_COLOUR									97
#define GLOBAL_PVS_ROOF_LIVERY										98
#define GLOBAL_PVS_DASHBOARD_COLOUR									99
#define GLOBAL_PVS_TYRES_MODE										102
#define GLOBAL_PVS_INSURANCE										/* iVehicleBS */ 103
#define GLOBAL_PVS_INSURANCE_OUT_GARAGE								/* MP_SAVED_VEHICLE_OUT_GARAGE */ 0
#define GLOBAL_PVS_INSURANCE_DESTROYED								/* MP_SAVED_VEHICLE_DESTROYED */ 1
#define GLOBAL_PVS_INSURANCE_INSURED								/* MP_SAVED_VEHICLE_INSURED */ 2

#define GLOBAL_FMBNR_SIZE											80
#define GLOBAL_FMBNR_STYLE											1
#define GLOBAL_FMBNR_STATE											2
#define GLOBAL_FMBNR_HEADLINE										21
#define GLOBAL_FMBNR_SUBTEXT										8
#define GLOBAL_FMBNR_CUSTOMTEXT_1									25
#define GLOBAL_FMBNR_CUSTOMTEXT_2									61
#define GLOBAL_FMBNR_CUSTOMTEXT_3									57
#define GLOBAL_FMBNR_CUSTOMINT_1									3
#define GLOBAL_FMBNR_CUSTOMINT_2									5
#define GLOBAL_FMBNR_PLAYER_1										17

#define iGangCallCurrentOwnerPlayer									106
#define iGangCallCurrentTargetPlayer								109

#define GlobalServerBD_FM_currentBounties							GlobalServerBD_FM + 4
#define GlobalServerBD_FM_currentBounties_size						3

// g_savedMPGlobalsNew.g_savedMPGlobals[GET_SAVE_GAME_ARRAY_SLOT()]
// Array size doesn't matter because we go into slot 0
#define GLOBAL_SAVE_GAME_ARRAY_SLOT									2359296 + 1
#define GLOBAL_MOST_RECENT_PV_INDEX									GLOBAL_SAVE_GAME_ARRAY_SLOT + 681 + 2 // && NETWORK::NETWORK_CLAN_PLAYER_IS_ACTIVE(uParam1)))
#define GLOBAL_BOUNTY_TIME_ELAPSED									GLOBAL_SAVE_GAME_ARRAY_SLOT + 5152 + 13 // >= 2880000)

#define GSBD_FM_IMPORTANT_ARRAY										GSBD_FM + 402 + 4

#define GLOBAL_INTERACT_SIZE										19
#define GLOBAL_INTERACT_iSceneId									3

#define GPBD_FM_4_SIZE												304
#define GPBD_FM_4_PICKUP_REQUESTS									101 + 21 // = SCRIPT::GET_HASH_OF_THIS_SCRIPT_NAME();
#define GPBD_FM_4_PICKUP_REQUESTS_SIZE								14

// am_pi_menu

#define GLOBAL_MP_PI_OPEN											MPGlobals + 990 + 6 // GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("MPInteraction", false);
#define GLOBAL_AM_PI_MENU_SUBMENU_CEO								27			// PI_BIK_0_0_Wx1
#define GLOBAL_AM_PI_MENU_SUBMENU_MC								54			// PI_BIK_0_0_Wx2

// appimportexport

#define GLOBAL_VEHICLE_EXPORT_COOLDOWN								GLOBAL_VIP_COOLDOWNS + 51 // "SHOW_EXPORT_SCREEN"

// am_mp_drone

#define GLOBAL_NANO_DRONE	1956920
#define GLOBAL_NANO_DRONE_RANGE_LIMIT	8

#pragma region ScrUpdate
// maintransition
#define g_SpawnData 2635563
#define g_SpawnData_bDontAskPermission g_SpawnData + 2997
#define GLOBAL_MAINTRANSID 1666552
#define g_TransitionSpawnData 2640096
#define g_TransitionSpawnData_bIgnoreSpawnActivity g_TransitionSpawnData + 18
#define g_sTransitionSessionData 2684504
#define g_sTransitionSessionData_bFmmcLauncherReady g_sTransitionSessionData + 851
#define GLOBAL_NEW_SESSION 1574589
#define GLOBAL_NEW_SESSION_QUIT_TO_SP GLOBAL_NEW_SESSION + 2
#define GLOBAL_CURRENT_SESSION_TYPE 1575036
#define GLOBAL_NEW_SESSION_TYPE GLOBAL_CURRENT_SESSION_TYPE - 1
#define GLOBAL_TRANS_SWITCH_1 1574633
#define GLOBAL_TRANS_SWITCH_2 18
#define GLOBAL_TRANS_SWITCH GLOBAL_TRANS_SWITCH_1 + GLOBAL_TRANS_SWITCH_2
#define GLOBAL_TRANS_CHECK_BYPASS_1 1574655
#define GLOBAL_TRANS_CHECK_BYPASS_2 6
#define GLOBAL_TRANS_CHECK_BYPASS GLOBAL_TRANS_CHECK_BYPASS_1 + GLOBAL_TRANS_CHECK_BYPASS_2
#define GLOBAL_TRANS_FLAGS 2621446
#define GLOBAL_MP_WORLD_STATE 2697886
#define MPGlobalsAmbience_g_bDoorsInited MPGlobalsAmbience + 4622
// am_launcher
#define GLOBAL_AM_LAUNCHER_HOST_DATA 2699172
// am_vehicle_spawn
#define GLOBAL_RECLAIM_DISABLED 1836674
// dialogue_handler
#define GLOBAL_DIAL_TIME 23307
// shop_controller
#define GLOBAL_SP_DESPAWN_BYPASS 4541512
// cellphone_controller
#define GLOBAL_INPUT_PHONE 20897
#define GLOBAL_DISABLE_PHONE 20913
// pi_menu
#define GLOBAL_SP_PI_OPEN 112916
// ingamehud
#define GLOBAL_LOADING_HINT 1577902
// social_controller
#define GLOBAL_AC_BAIL 110668
// timershud
#define GLOBAL_TIMERS 1670224
#define GLOBAL_TIMERS_BI_OFFSET 1163
// freemode
#define GLOBAL_PLAYERREMOTE_BASE /* GPBD_FM_3 */ 1887305
#define GLOBAL_PLAYERREMOTE_SIZE 610
#define GLOBAL_PLAYERREMOTE_BITSET1 1
#define GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK1 0 // Static
#define GLOBAL_PLAYERREMOTE_BITSET1_NOVOTEKICK2 7 // Static
#define GLOBAL_PLAYERREMOTE_ORG 10 // Static
#define GLOBAL_PLAYERREMOTE_SEVERIFY 512
#define GLOBAL_NEW_INVITES 23349
#define GLOBAL_CHAR_SLOT 1574926
#define GLOBAL_VIP_COOLDOWNS 2709334
#define GLOBAL_PASSIVE_KILL_COOLDOWN 1964652
#define GLOBAL_MKII_COOLDOWN 2710515
#define GLOBAL_VOTE_KICK_DATA_BASE 1874051
#define GLOBAL_VOTE_KICK_DATA_SIZE 100
#define GLOBAL_VOTE_KICKED 1877252
#define GLOBAL_VOTE_KICKED_DATA_SIZE 1
#define GLOBAL_DISABLE_VEHICLE_FOCUS 1582021
#define GLOBAL_CLAN_DESCS 1575115
#define GLOBAL_CLAN_DESCS_DATA_SIZE 35
#define GLOBAL_GIVE_WEAPONS 1578026
#define GLOBAL_BLIPS 2647061
#define GLOBAL_FADEOUTDEADBLIP GLOBAL_BLIPS + 373
#define GLOBAL_BLIPSTAGGERCOUNT GLOBAL_BLIPS + 1542
#define GLOBAL_BIGMAP GLOBAL_BLIPS + 1593
#define g_SimpleInteriorData 1943520
#define GLOBAL_LEAVE_INTERIOR g_SimpleInteriorData + 3632
#define GLOBAL_NEW_SESSION_UNK4 1575010
#define GLOBAL_PLAYER_STAT_BASE /* GlobalplayerBD_FM */ 1845281
#define GLOBAL_PLAYER_STAT_SIZE 883
#define GlobalServerBD_FM_events 1916617
#define GLOBAL_SEVERIFY GlobalServerBD_FM_events
#define GLOBAL_PLAYERSTATUS2_BASE /* GlobalplayerBD */ 2657971
#define GLOBAL_PLAYERSTATUS2_SIZE 465
#define GLOBAL_CEO_COLOURS_BASE GSBD + 818
#define GLOBAL_PLAYERSTATUS1_BASE 1668667
#define GLOBAL_IDLE_TIMER_1 1577923
#define GLOBAL_IDLE_TIMER_2 GLOBAL_PLAYERSTATUS1_BASE + 1154
#define GLOBAL_IDLE_TIMER_3 GLOBAL_PLAYERSTATUS1_BASE + 1170
#define GLOBAL_IDLE_TIMER_4 2698851
#define MPGlobalsAmbience 2738935
#define GLOBAL_PEGASUS_VEHICLE MPGlobalsAmbience + 307
#define GLOBAL_AM_AMMO_DROP MPGlobalsAmbience + 906
#define GLOBAL_AM_BOAT_TAXI MPGlobalsAmbience + 907
#define GLOBAL_AM_HELI_TAXI MPGlobalsAmbience + 908
#define MPGlobalsAmbience_bLaunchVehicleDropPersonal MPGlobalsAmbience + 943
#define MPGlobalsAmbience_iVDPersonalVehicleSlot MPGlobalsAmbience + 1000
#define MPGlobalsAmbience_bRequestedByOfficePA MPGlobalsAmbience + 1004
#define MPGlobalsAmbience_iPVSlotOfficePA MPGlobalsAmbience + 1007
#define GLOBAL_AM_BACKUP_HELI MPGlobalsAmbience + 4506
#define GLOBAL_AM_AIRSTRIKE MPGlobalsAmbience + 4507
#define GLOBAL_PASSIVE_COOLDOWN MPGlobalsAmbience + 4512
#define GLOBAL_EVENT_AUDIO_TRIGGERS MPGlobalsAmbience + 5083
#define GLOBAL_EWO_COOLDOWN MPGlobalsAmbience + 6975
#define GLOBAL_DISABLE_DEATH_BARRIERS MPGlobalsAmbience + 6958
#define GLOBAL_RC_PV_MODEL MPGlobalsAmbience + 6971
#define MPGlobals 2672855
#define GLOBAL_PLAYERS_JOINED_BITSET MPGlobals + 1
#define GLOBAL_OTRDEADLINE1 MPGlobals + 58
#define GLOBAL_OTRDEADLINE2 MPGlobals + 60
#define GLOBAL_PV_BLIP MPGlobals + 63
#define GLOBAL_PLAYER_PEGASUS_VEHICLE MPGlobals + 231
#define GLOBAL_RESPAWN_DATA MPGlobals + 1728
#define GLOBAL_FMBNR_BASE MPGlobals + 2557
#define GLOBAL_BST MPGlobals + 3733
#define GLOBAL_PVS 1586521
#define GLOBAL_PVS_ELMSIZE 142
#define g_MPRestrictedInteriorAccessData 1948680
#define g_MPRestrictedInteriorAccessData_iKickOutFromRestrictedInteriorBS g_MPRestrictedInteriorAccessData + 45
#define GLOBAL_BLOCK_C_IMPORTANT_ARRAY GSBD_BlockC + 1485
#define GlobalServerBD_BlockC_g_iAssignedCarMeetParkingSpots GSBD_BlockC + 2634
#define GLOBAL_LBD_PLAYER_COUNT 1836446
// am_pi_menu
#define GLOBAL_AM_PI_MENU_SUBMENU 2710429
#define GLOBAL_AM_PI_MENU_OPTION_ENABLED 2710430
// am_heli_taxi
#define MPGlobalsAmbience_iRequestedHeliByPA MPGlobalsAmbience + 915
// am_mp_drone
#define GLOBAL_NANO_DRONE_NET_TIMER 1956861
#pragma endregion ScrUpdate

#define GLOBAL_TUNABLE_BASE 262145

#pragma region ScriptGenie_Tunables
#define GLOBAL_TUNABLE_VOTE_KICKS_MULTIPLIER 6
#define GLOBAL_DISABLE_PV_INSURANCE_PAY GLOBAL_TUNABLE_BASE + 137
#define GLOBAL_TUNABLE_SNOW 4413
#define GLOBAL_TUNABLE_LESTER_BOUNTY_CUT 7179
#define GLOBAL_TUNABLE_WARMUP_CHALLENGES 11498
#define GLOBAL_TUNABLE_WARMUP_CRIMINAL_DAMAGE 11674
#define GLOBAL_ORBITAL_CANNON_COOLDOWN 22732
#define GLOBAL_TUNABLE_ALLOW_WEAPONIZED_VEHICLES_IN_PASSIVE_MODE 26732
#define GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_BASE 26393
#define GLOBAL_TUNABLE_LUCKY_WHEEL_ODDS_2_BASE 26413
#define GLOBAL_TUNABLE_LUCKY_WHEEL_ENABLE_REWARD_BASE 26499
#define GLOBAL_TUNABLE_PEYOTES 26683
#define GLOBAL_TUNABLE_DISABLE_DLC_INTRO_MOVIE 29587
#define GLOBAL_TUNABLE_CARMEET_REP_8_AMOUNT 30865
#define GLOBAL_TUNABLE_CARMEET_REP_8_MULTIPLIER 30967
#define GLOBAL_TUNABLE_BB_TERRORBYTE_DRONE_HEIGHT_LIMIT 24242
#define GLOBAL_KICK_OUT_OF_NIGHTCLUB_WHEN_ARMED GLOBAL_TUNABLE_BASE + 23844
#define GLOBAL_PASSIVE_EXIT_COOLDOWN GLOBAL_TUNABLE_BASE + 26735
#define GLOBAL_KOSATKA_MISSILE_COOLDOWN GLOBAL_TUNABLE_BASE + 29635
#define GLOBAL_KOSATKA_MISSILE_MAX_DISTANCE GLOBAL_TUNABLE_BASE + 29636
#define GLOBAL_CASINO_MAX_LOSS GLOBAL_TUNABLE_BASE + 26739

#define GLOBAL_ENABLE_HEIST_MASKS_HALLOWEEN 12020
#define GLOBAL_TURN_ON_HALLOWEEN_VEHICLES 12025
#define GLOBAL_TURN_ON_HALLOWEEN_MASKS 12026
#define GLOBAL_TURN_ON_HALLOWEEN_FACEPAINT 12027
#define GLOBAL_TURN_ON_HALLOWEEN_BOBBLEHEADS 12029
#define GLOBAL_TURN_ON_HALLOWEEN_CLOTHING 12035
#define GLOBAL_TURN_ON_HALLOWEEN_WEAPONS 12036
#define GLOBAL_TURN_ON_HALLOWEEN_HORNS 12037
#define GLOBAL_ENABLE_BIKER_SANCTUS 17366
#pragma endregion ScriptGenie_Tunables

#define REPORT_OFFTHREAD_SG_WRITE false

#include <functional>

#include <soup/Pointerlike.hpp>

namespace Stand
{
#if REPORT_OFFTHREAD_SG_WRITE
	inline thread_local bool g_sgtamper_readonly = false;
#endif

	inline void beginBadSgTamperCodeThatWillLandUsInHell()
	{
#if REPORT_OFFTHREAD_SG_WRITE
		g_sgtamper_readonly = true;
#endif
	}

	inline void endBadSgTamperCodeThatWillLandUsInHell()
	{
#if REPORT_OFFTHREAD_SG_WRITE
		g_sgtamper_readonly = false;
#endif
	}

	class ScriptGlobal : public soup::Pointerlike<ScriptGlobal>
	{
	private:
		uint32_t m_offset;

	public:
		explicit constexpr ScriptGlobal(uint32_t offset) noexcept
			: m_offset(offset)
		{
		}

		constexpr uint32_t offset() const noexcept
		{
			return m_offset;
		}

		[[nodiscard]] bool isAvailable() const noexcept;

		void* addr() const noexcept;

#if REPORT_OFFTHREAD_SG_WRITE
		template <typename T>
		[[nodiscard]] constexpr std::enable_if_t<!std::is_pointer_v<T>, T> get() const noexcept
		{
			g_sgtamper_readonly = true;
			T ret = *as<T*>();
			g_sgtamper_readonly = false;
			return ret;
		}

		template <typename T>
		[[nodiscard]] constexpr std::enable_if_t<std::is_pointer_v<T>&& std::is_const_v<std::remove_pointer_t<T>>, T> get() const noexcept
		{
			g_sgtamper_readonly = true;
			T ret = as<T>();
			g_sgtamper_readonly = false;
			return ret;
		}
#endif

		constexpr ScriptGlobal at(uint32_t a) const noexcept
		{
			return ScriptGlobal(offset() + a);
		}

		constexpr ScriptGlobal at(uint32_t i, uint32_t elm_size) const noexcept
		{
			return at(1 + (i * elm_size));
		}

		void iterate(uint32_t elm_size, const std::function<void(uint32_t, ScriptGlobal&&)>& consumer) const;

		[[nodiscard]] static uint32_t addrToOffset(void* addr);
	};
}
