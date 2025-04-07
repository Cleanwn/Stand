#include "Hooking.hpp"

#include <sstream>

#include <fmt/core.h>
#include <MinHook.h>
#include <rapidjson/document.h>

#include <soup/base64.hpp>
#include <soup/json.hpp>
#include <soup/ObfusString.hpp>
#include <soup/Pattern.hpp>
#include <soup/rand.hpp>
#include <soup/string.hpp>
#include <soup/unicode.hpp>

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "atArray.hpp"
#include "atFixedArray.hpp"
#include "BgScript.hpp"
#include "Blacklist.hpp"
#include "CExplosionEvent.hpp"
#include "Chat.hpp"
#include "evtChatEvent.hpp"
#include "CClearAreaEvent.hpp"
#include "CGiveWeaponEvent.hpp"
#include "CloudEvent.hpp"
#include "ColoadMgr.hpp"
#include "ComponentCrashPatch.hpp" // sync_src
#include "ComponentNetcode.hpp" // packet_src
#include "Components.hpp"
#include "CMultiplayerChat.hpp"
#include "CMsgTextMessage.hpp"
#include "CNetGamePlayer.hpp"
#include "CNetGamePlayerDataMsg.hpp"
#include "CNetworkPlaySoundEvent.hpp"
#include "CNetworkPtFXEvent.hpp"
#include "CNetworkPtFXWorldStateData.hpp"
#include "CommandBlockBlockJoin.hpp"
#include "CommandDlcpacks.hpp"
#include "CommandDriveOnWater.hpp"
#include "CommandList.hpp"
#include "CommandLuaScript.hpp"
#include "CommandPlayer.hpp"
#include "CommandPlayerMagnet.hpp"
#include "CObject.hpp"
#include "CPedModelInfo.hpp"
#include "CPlayerInfo.hpp"
#include "CProjectile.hpp"
#include "CRemoveWeaponEvent.hpp"
#include "CStartProjectileEvent.hpp"
#if CLEAR_BONUS_ON_DL
#include "CTunables.hpp"
#endif
#include "CustomDlcMgr.hpp"
#include "CVehicleModelInfo.hpp"
#include "CWeaponDamageEvent.hpp"
#include "datBitBuffer.hpp"
#include "DirectPacket.hpp"
#include "eBailReason.hpp"
#include "eEntityType.hpp"
#include "eExplosionTag.hpp"
#include "ExecCtx.hpp"
#include "eEventType.hpp"
#include "eTaskType.hpp"
#include "EventLabel.hpp"
#include "EventTally.hpp"
#include "evtEvent.hpp"
#include "evtPadShakeEvent.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "fmt_arg.hpp"
#include "format_time_since_1970_for_user_locale.hpp"
#include "FriendsReaderTask.hpp"
#include "fwInteriorLocation.hpp"
#include "gameSkeleton.hpp"
#include "get_current_time_millis.hpp"
#include "get_seconds_since_unix_epoch.hpp"
#include "gsType.hpp"
#include "gta_cam.hpp"
#include "gta_event.hpp"
#include "gta_http.hpp"
#include "gta_input.hpp"
#include "gta_matchmaking.hpp"
#include "gta_metrics.hpp"
#include "gta_net_game_event.hpp"
#include "gta_net_object_mgr.hpp"
#include "gta_ped.hpp"
#include "gta_session_tasks.hpp"
#include "gta_sync_data.hpp"
#include "gta_task.hpp"
#include "gta_vehicle.hpp"
#include "gta_vehicle_layout.hpp"
#include "Gui.hpp"
#include "HttpRequestBuilder.hpp"
#include "IncrementableStat.hpp"
#include "is_rid.hpp"
#include "is_session.hpp"
#include "joaat_hash_db.hpp"
#include "joaatToString.hpp"
#include "lang.hpp"
#include "LeaveReasons.hpp"
#include "FileLogger.hpp"
#include "main.hpp"
#include "Matchmaking.hpp"
#include "mystackwalker.hpp"
#include "natives.hpp"
#include "net_array.hpp"
#include "netConnection.hpp"
#include "netEndpoint.hpp"
#include "netMessageId.hpp"
#include "NetScriptWorldStateTypes.hpp"
#include "netTaskStatus.hpp"
#include "NuancedEventWithExtraData.hpp"
#include "OrgHelper.hpp"
#include "PlayerHistory.hpp"
#include "pointers.hpp"
#include "RageConnector.hpp"
#include "regular_event.hpp"
#include "RemoteGamer.hpp"
#include "Renderer.hpp"
#include "rgsc_profiles.hpp"
#include "rlGamerHandle.hpp"
#include "rlGetGamerStateTask.hpp"
#include "rlpresence.hpp"
#include "rlSessionDetail.hpp"
#include "ScAccount.hpp"
#include "ScriptErrorRecoveryMode.hpp"
#include "script_events.hpp"
#include "script_program.hpp"
#include "ScriptVmErrorHandling.hpp"
#include "script_vm_reimpl.hpp"
#include "ScriptEventTaxonomy.hpp"
#include "ScriptGlobal.hpp"
#include "script_handler.hpp"
#include "ScriptMgr.hpp"
#include "script_thread.hpp"
#include "script_vm_common.hpp"
#include "snEvent.hpp"
#include "StatCache.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"
#include "synctree_player.hpp"
#include "sysDependency.hpp"
#include "TpUtil.hpp"
#include "TransitionState.hpp"
#include "Tunables.hpp"
#include "Util.hpp"
#include "VehicleGadgets.hpp"
#include "VehicleType.hpp"
#include "weapons.hpp"
#include "weapon_components.hpp"

#define PROFILE_SCRIPTS			false
#define PROFILE_HISTORY_LOOKUP	false
#define PRINT_MMREQUESTS		false

#ifdef STAND_DEV
#define DEBUG_SYNCTREE_BUFFER_OVERRUNS true
#define DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS true
#else
#define DEBUG_SYNCTREE_BUFFER_OVERRUNS false
#define DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS false
#endif

#if PROFILE_SCRIPTS
#include "drawDebugText.hpp"
#endif

#if PROFILE_HISTORY_LOOKUP
static_assert(STAND_DEBUG);
#include "TimedCall.hpp"
#endif

#if PRINT_MMREQUESTS
static_assert(STAND_DEBUG);
#include "rl_matching.hpp"
#endif

#if DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS
static_assert(STAND_DEV); // for rage_netSyncDataNode_Read_hook
#include "CNetworkPlayerMgr.hpp"
#endif

namespace Stand
{
	struct EventStat
	{
		const Hash name;
		const Hash hash;
	};

	static constexpr EventStat event_stats[] = {
		// Commendations:
		{ATSTRINGHASH("ESTAT_0"), STAT_HELPFUL},
		{ATSTRINGHASH("ESTAT_1"), STAT_FRIENDLY},
		// Reports:
		//{ATSTRINGHASH("ISE_VC_A"), STAT_VC_ANNOYINGME},
		//{ATSTRINGHASH("ISE_VC_H"), STAT_VC_HATE},
		//{ATSTRINGHASH("ISE_G2"), STAT_GRIEFING},
		//{ATSTRINGHASH("ISE_X2"), STAT_EXPLOITS},
		//{ATSTRINGHASH("ISE_B2"), STAT_GAME_EXPLOITS},
		//{ATSTRINGHASH("ISE_TC_A"), STAT_TC_ANNOYINGME},
		//{ATSTRINGHASH("ISE_TC_H"), STAT_TC_HATE},
		//{ATSTRINGHASH("ISE_VG"), 491618849},
		//{ATSTRINGHASH("ISE_ONC"), -203536263},
	};

	static rage::SyncType current_sync_type{};
	static bool sync_was_read = false;
	static rage::netSyncTree* current_sync_tree;
	static rage::NetworkObjectType current_create_object_type{};
	static uint16_t current_create_object_id{};
	static std::unordered_set<uint16_t> resync_requested{};

	extern "C"
	{
		// script_vm_helper.asm
		void script_vm_helper_reset_ins_count();
		void script_vm_switch_detour();
		void script_vm_error_detour();

		// swapchain_hook.asm
		void swapchain_hook_present();
		void swapchain_hook_resize_buffers();

		// data_node_helper.asm
		void data_node_write_detour();

		// rage_input_helper.asm
		void set_value_from_keyboard_detour();
		void set_value_from_mkb_axis_detour();

		// aes_helper.asm
		void aes_initfile_detour();
		void aes_decrypt_callsite_detour();

		// overrun_helper.asm
		void net_event_error_pre_memmove_detour();
	}

#define FOR_EACH_TRIVIAL_HOOK(f) \
f("B9", rage_scrThread_UpdateAll, DH_MANDATORY) /* critical for menu functionality */ \
f("C0", send_CMsgTextMessage, DH_MANDATORY) \
f("C1", received_chat_message, DH_MANDATORY) \
f("C3", CMultiplayerChat_UpdateInputWhenTyping, DH_MANDATORY) \
f("C6", scaleform_is_key_pressed_1, DH_MANDATORY) \
f("C7", is_chat_character_valid, 0) \
f("CA", CNetworkTextChat_SubmitText, DH_MANDATORY) \
f("P0", rage_netEventMgr_ProcessEventData, DH_MANDATORY) \
f("BW", CEventGroupScriptNetwork_DispatchEvent, DH_MANDATORY) \
f("PB", script_vm, DH_MANDATORY) /* critical for script vm error handling */ \
f("Q4", CNetworkSession_OnSessionEvent, DH_MANDATORY) \
f("R0", rlGamerHandle_from_friend_index, 0) \
f("R4", get_matchmaking_pool, 0) \
f("R3", CExtraContentManager_GetCRC, 0) \
f("RO", rage_rlGetGamerStateTask_ParseResults, DH_MANDATORY) \
f("S7", get_player_card_stat, 0) \
f("S8", CPlayerGamerDataNode_Serialise, 0) \
f("SA", CNetObjVehicle_GetVehicleMigrationData, 0) \
f("PI", received_clone_create, DH_MANDATORY) \
f("PJ", received_clone_sync, DH_MANDATORY) \
f("PP", sync_can_apply, DH_MANDATORY) \
f("N2", rage_netEventMgr_SendEvent, DH_MANDATORY) \
f("NF", network_can_access_multiplayer, 0) \
/*f("RD", rage_rlScMatchmakingFindTask_ReadSession, 0)*/ \
/*f("RE", rage_rlScMatchmakingFindTask_ProcessSuccess, 0)*/ \
f("FO", rage_rlScMatchmaking_Advertise, 0) \
f("FP", rage_rlScMatchmaking_Unadvertise, 0) \
f("FQ", rage_netTransactor_SendResponse_rage_rlSessionDetailResponse, 0) \
f("PF", received_clone_create_ack, 0) \
f("PG", received_clone_sync_ack, 0) \
f("PH", received_clone_remove_ack, 0) \
f("PK", clone_pack, 0) \
/*f("PL", send_clone_create, 0)*/ \
f("PM", send_clone_sync, 0) \
/*f("PN", send_clone_remove, 0)*/ \
f("PO", rage_netSyncTree_Read, 0) \
f("PV", CNetObjVehicle_SetVehicleControlData, 0) \
f("Q1", InviteMgr_AcceptInvite, 0) \
/*f("RJ", CNetworkSession_JoinSession, 0)*/ \
f("RK", rage_playerDataMsg_SerMsgPayload_datExportBuffer, 0) \
f("RL", rage_netArrayHandlerBase_WriteUpdate, 0) \
f("Q7", CNetworkSession_OnHandleJoinRequest, 0) \
f("QR", rage_netIceSession_GetAdditionalLocalCandidates, 0) \
f("D2", swapchain_present_streamproof, DH_NOFOLLOWJUMPS) \
f("L0", CTextFile_Get, 0) /* critical for OG call */ \
f("L1", CTextFile_GetInternal, DH_MANDATORY) /* critical for OG call */ \
f("P5", rage_netPlayerMgrBase_SendBuffer, 0) \
f("P7", rage_fwArchetypeManager_GetArchetypeFromHashKey, 0) /* critical for OG call */ \
f("BY", camFrame_copy, 0) \
f("FK", CPhysical_TestNoCollision, 0) \
f("G0", CProjectileManager_CreateProjectile, 0) \
f("PQ", parse_presence_event, 0) \
f("RQ", rgsc_RgscPresenceManager_OnSocialClubEvent, 0) \
f("PW", rage_ropeManager_Remove, 0) \
f("V2", get_visual_config_float, 0) \
f("V3", get_visual_config_float_with_directory, 0) \
f("V1", get_visual_config_colour, 0) \
f("V4", camFactory_CreateObject, 0) \
f("X0", some_player_sync, 0) \
f("X1", rage_aiTaskTree_UpdateTask, 0) \
f("F2", CControl_StartPlayerPadShake, 0) \
f("F5", rage_FriendsReaderTask_Complete, 0) \
f("O5", CModelInfo_AddVehicleModel, 0) \
f("OA", CNetObjVehicle_GetVehicleCreateData, 0) \
/**/

#define FOR_EACH_HOOK_FROM_POINTERS(f) \
/*f("RC", rage_rlScMatchmaking_Find, DH_MANDATORY)*/ \
/**/

	Hooking::Hooking() :
		flow_event_reactions{
			{}, // OBJECT_ID_FREED_EVENT
			{}, // OBJECT_ID_REQUEST_EVENT
			{}, // ARRAY_DATA_VERIFY_EVENT
			{}, // SCRIPT_ARRAY_DATA_VERIFY_EVENT
			{}, // REQUEST_CONTROL_EVENT
			{}, // GIVE_CONTROL_EVENT
			{}, // WEAPON_DAMAGE_EVENT
			{}, // REQUEST_PICKUP_EVENT
			{}, // REQUEST_MAP_PICKUP_EVENT
			{}, // 9 - not assigned
			{}, // 10 - not assigned
			{}, // RESPAWN_PLAYER_PED_EVENT
			{}, // GIVE_WEAPON_EVENT
			{}, // REMOVE_WEAPON_EVENT
			{}, // REMOVE_ALL_WEAPONS_EVENT
			{}, // VEHICLE_COMPONENT_CONTROL_EVENT
			{}, // FIRE_EVENT
			{}, // EXPLOSION_EVENT
			{}, // START_PROJECTILE_EVENT
			{}, // UPDATE_PROJECTILE_TARGET_EVENT
			{}, // BREAK_PROJECTILE_TARGET_LOCK_EVENT
			{}, // REMOVE_PROJECTILE_ENTITY_EVENT
			{}, // ALTER_WANTED_LEVEL_EVENT
			{}, // CHANGE_RADIO_STATION_EVENT
			{}, // RAGDOLL_REQUEST_EVENT
			{}, // PLAYER_TAUNT_EVENT
			{}, // PLAYER_CARD_STAT_EVENT
			{}, // DOOR_BREAK_EVENT
			{}, // SCRIPTED_GAME_EVENT
			{}, // REMOTE_SCRIPT_INFO_EVENT
			{}, // REMOTE_SCRIPT_LEAVE_EVENT
			{}, // MARK_AS_NO_LONGER_NEEDED_EVENT
			{}, // CONVERT_TO_SCRIPT_ENTITY_EVENT
			{}, // SCRIPT_WORLD_STATE_EVENT
			{}, // INCIDENT_ENTITY_EVENT
			{}, // CLEAR_AREA_EVENT
			{}, // CLEAR_RECTANGLE_AREA_EVENT
			{}, // NETWORK_REQUEST_SYNCED_SCENE_EVENT
			{}, // NETWORK_START_SYNCED_SCENE_EVENT
			{}, // NETWORK_UPDATE_SYNCED_SCENE_EVENT
			{}, // NETWORK_STOP_SYNCED_SCENE_EVENT
			{}, // GIVE_PED_SCRIPTED_TASK_EVENT
			{}, // GIVE_PED_SEQUENCE_TASK_EVENT
			{}, // NETWORK_CLEAR_PED_TASKS_EVENT
			{}, // NETWORK_START_PED_ARREST_EVENT
			{}, // NETWORK_START_PED_UNCUFF_EVENT
			{}, // NETWORK_SOUND_CAR_HORN_EVENT
			{}, // NETWORK_ENTITY_AREA_STATUS_EVENT
			{}, // NETWORK_GARAGE_OCCUPIED_STATUS_EVENT
			{}, // PED_CONVERSATION_LINE_EVENT
			{}, // SCRIPT_ENTITY_STATE_CHANGE_EVENT
			{}, // NETWORK_PLAY_SOUND_EVENT
			{}, // NETWORK_STOP_SOUND_EVENT
			{}, // NETWORK_PLAY_AIRDEFENSE_FIRE_EVENT
			{}, // NETWORK_BANK_REQUEST_EVENT
			{}, // NETWORK_AUDIO_BARK_EVENT
			{}, // REQUEST_DOOR_EVENT
			{}, // NETWORK_TRAIN_REQUEST_EVENT
			{}, // NETWORK_TRAIN_REPORT_EVENT
			{}, // NE_ANY (NETWORK_INCREMENT_STAT_EVENT)
			{}, // MODIFY_VEHICLE_LOCK_WORD_STATE_DATA
			{}, // MODIFY_PTFX_WORD_STATE_DATA_SCRIPTED_EVOLVE_EVENT
			{}, // REQUEST_PHONE_EXPLOSION_EVENT
			{}, // REQUEST_DETACHMENT_EVENT
			{}, // KICK_VOTES_EVENT
			{}, // GIVE_PICKUP_REWARDS_EVENT
			{}, // BLOW_UP_VEHICLE_EVENT
			{}, // NETWORK_SPECIAL_FIRE_EQUIPPED_WEAPON
			{}, // NETWORK_RESPONDED_TO_THREAT_EVENT
			{}, // NETWORK_SHOUT_TARGET_POSITION
			{}, // VOICE_DRIVEN_MOUTH_MOVEMENT_FINISHED_EVENT
			{}, // PICKUP_DESTROYED_EVENT
			{}, // UPDATE_PLAYER_SCARS_EVENT
			{}, // NETWORK_CHECK_EXE_SIZE_EVENT
			{}, // NETWORK_PTFX_EVENT
			{}, // NETWORK_PED_SEEN_DEAD_PED_EVENT
			{}, // REMOVE_STICKY_BOMB_EVENT
			{}, // NETWORK_CHECK_CODE_CRCS_EVENT
			{}, // INFORM_SILENCED_GUNSHOT_EVENT
			{}, // PED_PLAY_PAIN_EVENT
			{}, // CACHE_PLAYER_HEAD_BLEND_DATA_EVENT
			{}, // REMOVE_PED_FROM_PEDGROUP_EVENT
			{}, // REPORT_MYSELF_EVENT
			{ REACTION_BLOCK }, // REPORT_CASH_SPAWN_EVENT
			{}, // ACTIVATE_VEHICLE_SPECIAL_ABILITY_EVENT
			{}, // BLOCK_WEAPON_SELECTION
			{}, // NETWORK_CHECK_CATALOG_CRC
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // NE_VEHTAKEOVER
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // NE_INFRING
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // NE_CAMSHAKE
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // NE_EXPSPAM
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // NE_RAGDOLL
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // NE_SUDDENDEATH
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // NE_DISARM
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK | REACTION_TIMEOUT }, // SE_CRASH
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_KICK
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_PVKICK
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_SENDTOJOB
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_CEOKICK
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_STARTFMMISSION
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_STARTFMMISSION_NCEO
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_INTERIORWARP
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_INTERIORWARP_NCEO
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_GIVECOLLE
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_GIVECOLLE_NCEO
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_INVALID
			{ REACTION_TOAST | REACTION_LOG_CONSOLE, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_UNUSUAL
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_APTINV
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_LOG_FILE | REACTION_BLOCK }, // SE_INFLOAD
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_DISABLEDRIVINGVEHICLES
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_CAYO_INV
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_FORCECAMFRONT
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_FREEZE
			{ REACTION_BLOCK }, // SE_NOTIFY_EMPTY
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SE_INTERIORKICK
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SE_TRIGGERDEFEND
			{}, // PUP_ANY
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // PUP_INVALID
			{}, // PUP_RP
			{}, // PUP_CASH
			{}, // SYNCIN_ANY
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_LOG_FILE | REACTION_BLOCK }, // SYNCIN_IMS
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SYNCIN_WOS
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SYNCIN_CAGE
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK | REACTION_TIMEOUT }, // SYNCIN_ATTACH
			{}, // SYNCIN_ACK_CLONE_CREATE
			{}, // SYNCIN_ACK_CLONE_UPDATE
			{}, // SYNCIN_ACK_CLONE_DELETE
			{}, // SYNCIN_CLONE_CREATE
			{}, // SYNCIN_CLONE_UPDATE
			{}, // SYNCIN_CLONE_DELETE
			{}, // SYNCIN_CONTROL
			{}, // SYNCOUT_CLONE_CREATE
			{}, // SYNCOUT_CLONE_UPDATE
			{}, // SYNCOUT_CLONE_DELETE
			{}, // MOD_ANY
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_ID_STAND
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_ID_STAND_COLOADING
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_ID_OTHER
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_RAC
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_HEALTH
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_LONG_DEATH
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_LONG_OTR
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_ARMOR
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_PRESENCESPOOFING
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_HOSTKN_HANDICAP
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_HOSTKN_SWEET
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_HOSTKN_AGGRESSIVE
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_SPFDHOSTKN
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_GODATK
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_FROG
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_RSDEV
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_RSQA
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_CHEAT
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_RP2
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_RP3
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_RP4
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_T2IP
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_ADMINRID
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_MODEL
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_DMGMUL
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_NONET
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_DRIVEB
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_SILENTCHAT
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_BOUNTYSPAM
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_SUSSYBOUNTY
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_EXPBLAME
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_BADEXPLOSION
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // MOD_ORGNAME
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_CAYO
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_DEEZNATS
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_MISC
#ifdef STAND_DEBUG
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_INCONGRUENCE_1
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // MOD_INCONGRUENCE_2
#endif
			{}, // CHAT_ANY
			{ 0, 0, REACTION_BLOCK }, // CHAT_AD
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // CHAT_PROFANITYBYPASS
			{}, // CHAT_SPAM
			{}, // SMS_ANY
			{ 0, 0, REACTION_BLOCK }, // SMS_AD
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // SMS_PROFANITYBYPASS
			{}, // CLSFN_ANY
			{}, // CLSFN_PM
			{}, // CLSFN_LM
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // CLSFN_MOD
#if HAS_MODADM
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // CLSFN_MODAMN
#endif
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_LOG_FILE }, // CLSFN_AMN
			{}, // SCRLAUNCH_ANY
			{}, // SCRLAUNCH_NOCAT
			{}, // SCRLAUNCH_FE
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_AG
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE }, // SCRLAUNCH_OFM
			{ REACTION_BLOCK, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_GRIEF
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_SERV
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_CUT
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_FOIM
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_PS
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_STRIKE
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_DPSSV
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_DARTS
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_IMPDM
			{ 0, REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // SCRLAUNCH_SLASHER
			{}, // CUSTOM_1
			{}, // CUSTOM_2
			{}, // CUSTOM_3
			{}, // CUSTOM_4
			{}, // CUSTOM_5
			{}, // CUSTOM_6
			{}, // CUSTOM_7
			{}, // CUSTOM_8
			{}, // CUSTOM_9
			{}, // CUSTOM_10
			{}, // CUSTOM_11
			{}, // CUSTOM_12
			{}, // CUSTOM_13
			{}, // CUSTOM_14
			{}, // CUSTOM_15
			{}, // CUSTOM_16
			{}, // CUSTOM_17
			{}, // CUSTOM_18
			{}, // CUSTOM_19
			{}, // CUSTOM_20
			{}, // MISC_JOIN
			{}, // MISC_HOSTCHANGE
			{}, // MISC_SCRIPTHOSTCHANGE
			{}, // MISC_VOTEKICK_ANY
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // MISC_VOTEKICK_ME
			{}, // MISC_LOVELETTER
			{ REACTION_TOAST | REACTION_LOG_CONSOLE }, // MISC_BOUNTY
			{ REACTION_TOAST | REACTION_LOG_CONSOLE | REACTION_BLOCK }, // MISC_PTFX
		},

#define ITERATOR_CTOR(code, name, flags) name ## _hook(code, &hooks::name, flags),
		FOR_EACH_TRIVIAL_HOOK(ITERATOR_CTOR)
		FOR_EACH_HOOK_FROM_POINTERS(ITERATOR_CTOR)

#if HTTP_HOOK
		rage_netHttpRequest_Update_hook("N1", &hooks::rage_netHttpRequest_Update, DH_MANDATORY),
#endif

#if CLEAR_BONUS_ON_DL
		CTunables_OnCloudEvent_hook("N5", &hooks::CTunables_OnCloudEvent, 0),
#endif

#ifdef STAND_DEBUG
		rage_netMessage_WriteHeader_hook("Z0", &hooks::rage_netMessage_WriteHeader, 0),
#endif
#ifdef STAND_DEV
		rage_netSyncDataNode_Read_hook("Z1", &hooks::rage_netSyncDataNode_Read, 0),
#endif

		rage_fwConfigManager_GetSizeOfPool_hook("A1", &hooks::rage_fwConfigManager_GetSizeOfPool, 0),

		swapchain_resize_buffers_hook("D1", &swapchain_hook_resize_buffers, 0),
		swapchain_present_hook("D0", &swapchain_hook_present, DH_NOFOLLOWJUMPS | DH_MANDATORY),

		set_value_from_keyboard_hook("I0", &set_value_from_keyboard_detour, DH_NOFOLLOWJUMPS),
		set_value_from_mkb_axis_hook("I1", &set_value_from_mkb_axis_detour, DH_NOFOLLOWJUMPS),

		script_vm_switch_hook("PC", &script_vm_switch_detour, DH_NOFOLLOWJUMPS),
		script_thread_error_kill_1_hook("PD", &script_vm_error_detour, DH_NOFOLLOWJUMPS),
		script_thread_error_kill_2_hook("PE", &script_vm_error_detour, DH_NOFOLLOWJUMPS),

		rage_rlTelemetry_Export_hook("N0", &hooks::rage_rlTelemetry_Export, 0),
		rage_gameSkeleton_updateGroup_Update_hook("N8", &hooks::rage_gameSkeleton_updateGroup_Update, DH_NOFOLLOWJUMPS),
		network_bail_hook("N9", &hooks::network_bail, 0),
		rage_sysDependencyScheduler_InsertInternal_hook("NE", &hooks::rage_sysDependencyScheduler_InsertInternal, DH_NOFOLLOWJUMPS),

		aes_initfile_hook("O1", &aes_initfile_detour, DH_NOFOLLOWJUMPS),
		aes_decrypt_callsite_hook("O3", &aes_decrypt_callsite_detour, DH_NOFOLLOWJUMPS),

		send_net_info_to_lobby_wrap_hook("S0", &hooks::send_net_info_to_lobby_wrap, DH_MANDATORY),
		CNetGamePlayer_GetGamerInfo_hook("S3", &hooks::CNetGamePlayer_GetGamerInfo, DH_NOFOLLOWJUMPS),
		get_active_clan_data_hook("S4", &hooks::get_active_clan_data, DH_MANDATORY),
		data_node_write_hook("S9", &data_node_write_detour, DH_NOFOLLOWJUMPS),

		net_event_error_pre_memmove_hook("P9", &net_event_error_pre_memmove_detour, DH_NOFOLLOWJUMPS)
	{
		initSpoofedClan();
		spoofed_clan_membership.clan.id = 133742069;
		spoofed_clan_membership.clan.alt_badge = true;
		strcpy(spoofed_clan_membership.clan.name, soup::ObfusString("Stand.sh").c_str());
		strcpy(spoofed_clan_membership.clan.tag, "STD");
		strcpy(spoofed_clan_membership.position, "Member");
	}

	void Hooking::initSpoofedClan()
	{
		spoofed_clan_membership.member_id = 226986120;
		spoofed_clan_membership.unk_2 = 2058;
		spoofed_clan_membership.is_active = true;
	}

// When adding a new hook here, make sure "found that Stand is already injected" is preserved.
#define FOR_EACH_LONGJUMP_HOOK(f) \
f(rage_scrThread_UpdateAll_hook) \
f(received_chat_message_hook) \
f(send_net_info_to_lobby_wrap_hook) \
f(rage_gameSkeleton_updateGroup_Update_hook) \
f(network_bail_hook) \
f(rage_sysDependencyScheduler_InsertInternal_hook) \
f(rage_rlGetGamerStateTask_ParseResults_hook) \
/*f(rage_rlScMatchmaking_Find_hook)*/ \
/*f(rage_rlScMatchmakingFindTask_ReadSession_hook)*/ \
/*f(rage_rlScMatchmakingFindTask_ProcessSuccess_hook)*/ \
f(rage_netArrayHandlerBase_WriteUpdate_hook) \
/**/

	void Hooking::init()
	{
#define INIT_LONGJUMP_HOOK(hook) hook.flags |= DH_LONGJUMP;
		FOR_EACH_LONGJUMP_HOOK(INIT_LONGJUMP_HOOK);

#if HTTP_HOOK
		INIT_LONGJUMP_HOOK(rage_netHttpRequest_Update_hook);
#endif

		MH_Initialize();
	}

	void Hooking::deinit()
	{
#define DEINIT_LONGJUMP_HOOK(hook) if (hook.isHooked() && hook.isLongjump()) { hook.removeHook(); }
		FOR_EACH_LONGJUMP_HOOK(DEINIT_LONGJUMP_HOOK);

#if HTTP_HOOK
		DEINIT_LONGJUMP_HOOK(rage_netHttpRequest_Update_hook);
		redir_host.clear();
#endif

		MH_Uninitialize();
	}

	void Hooking::setTargets()
	{
#define ITERATOR_INIT(code, name, flags) name##_hook.setTarget(pointers::name);
		FOR_EACH_HOOK_FROM_POINTERS(ITERATOR_INIT)

		swapchain_resize_buffers_hook.setTarget(pointers::swapchain_resize_buffers);
		swapchain_present_hook.setTarget(pointers::swapchain_present);

		script_vm_switch_hook.setTarget(pointers::script_vm_switch);
		script_thread_error_kill_1_hook.setTarget(pointers::script_thread_error_kill_1);
		script_thread_error_kill_2_hook.setTarget(pointers::script_thread_error_kill_2);

		get_active_clan_data_hook.setTarget(pointers::rage_rlClan_GetPrimaryMembership);
	}

	void Hooking::performChecks()
	{
#ifdef STAND_DEBUG
		if (flow_event_reactions[FlowEvent::MISC_VOTEKICK_ME].getReactions(PlayerType::STRANGER) != (REACTION_TOAST | REACTION_LOG_CONSOLE))
		{
			Util::toast("Reactions sanity check failed", TOAST_ALL);
		}
#endif
	}

	std::vector<DetourHook*> Hooking::getMinimalHooks()
	{
		return {
			&swapchain_resize_buffers_hook,
			&swapchain_present_hook,
			&send_net_info_to_lobby_wrap_hook,
			&CNetGamePlayer_GetGamerInfo_hook,
			&get_active_clan_data_hook,

			&rage_rlTelemetry_Export_hook,
			&rage_gameSkeleton_updateGroup_Update_hook,
			&network_bail_hook,
			&rage_sysDependencyScheduler_InsertInternal_hook,
		};
	}

#if ENABLE_PASSIVE_DLL
	std::vector<DetourHook*> Hooking::getPassiveHooks()
	{
		return {
#if HTTP_HOOK
			&rage_netHttpRequest_Update_hook,
#endif
		};
	}
#endif

	std::vector<DetourHook*> Hooking::getMainHooks()
	{
		std::vector<DetourHook*> main_hooks = {
			&script_vm_switch_hook,
			&script_thread_error_kill_1_hook,
			&script_thread_error_kill_2_hook,

			&data_node_write_hook,

			&set_value_from_keyboard_hook,
			&set_value_from_mkb_axis_hook,

			&net_event_error_pre_memmove_hook,

			&aes_initfile_hook,
			&aes_decrypt_callsite_hook,

#define ITERATOR_GET(code, name, flags) &name##_hook,
			FOR_EACH_TRIVIAL_HOOK(ITERATOR_GET)
			FOR_EACH_HOOK_FROM_POINTERS(ITERATOR_GET)

#if CLEAR_BONUS_ON_DL
			&CTunables_OnCloudEvent_hook,
#endif

#ifdef STAND_DEBUG
			&rage_netMessage_WriteHeader_hook,
#endif
#ifdef STAND_DEV
			&rage_netSyncDataNode_Read_hook,
#endif
		};
		Components::collectHooks(main_hooks);
		return main_hooks;
	}

	static void removeNonHookedHooks(std::vector<DetourHook*>& hooks)
	{
		for (auto i = hooks.begin(); i != hooks.end(); )
		{
			if ((*i)->isHooked())
			{
				++i;
			}
			else
			{
				i = hooks.erase(i);
			}
		}
	}

	std::vector<DetourHook*> Hooking::getAllHooks()
	{
		std::vector<DetourHook*> hooks = {
			&g_hooking.rage_fwConfigManager_GetSizeOfPool_hook
		};
		mergeHooks(hooks, getMinimalHooks());
#if ENABLE_PASSIVE_DLL
		mergeHooks(hooks, getPassiveHooks());
#endif
		mergeHooks(hooks, getMainHooks());
		return hooks;
	}

#if ENABLE_PASSIVE_DLL
	std::vector<DetourHook*> Hooking::getNonPassiveHooks()
	{
		std::vector<DetourHook*> hooks = {
			&g_hooking.rage_fwConfigManager_GetSizeOfPool_hook
		};
		mergeHooks(hooks, getMinimalHooks());
		mergeHooks(hooks, getMainHooks());
		return hooks;
	}
#endif

	std::vector<DetourHook*> Hooking::getHookedHooks()
	{
		std::vector<DetourHook*> hooks = getAllHooks();
		removeNonHookedHooks(hooks);
		return hooks;
	}

	void Hooking::batchCreateHooks(const std::vector<DetourHook*>& hooks)
	{
		for (const auto& hook : hooks)
		{
			hook->createHook();
		}
	}

	void Hooking::batchEnableHooks(const std::vector<DetourHook*>& hooks)
	{
		for (const auto& hook : hooks)
		{
			hook->enableHookQueued();
		}
		MH_ApplyQueued();
	}

	void Hooking::batchDisableHooks(const std::vector<DetourHook*>& hooks)
	{
		for (const auto& hook : hooks)
		{
			hook->disableHookQueued();
		}
		MH_ApplyQueued();
	}

	void Hooking::batchRemoveHooks(const std::vector<DetourHook*>& hooks)
	{
		for (const auto& hook : hooks)
		{
			hook->removeHook();
		}
	}

	void Hooking::batchDisableAndRemoveHooks(const std::vector<DetourHook*>& hooks)
	{
		batchDisableHooks(hooks);
		batchRemoveHooks(hooks);
	}

	void Hooking::hookWndproc()
	{
		if (og_wndproc == nullptr)
		{
			og_wndproc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(pointers::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&hooks::wndproc)));
		}
	}

	void Hooking::unhookWndproc()
	{
		if (og_wndproc != nullptr)
		{
			SetWindowLongPtrW(pointers::hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(og_wndproc));
			og_wndproc = nullptr;
		}
	}

	void Hooking::disableAll()
	{
#define DISABLE_LONGJUMP_HOOK(hook) if (hook.isHooked() && hook.isLongjump()) { hook.disableHook(); }
		FOR_EACH_LONGJUMP_HOOK(DISABLE_LONGJUMP_HOOK);

#if HTTP_HOOK
		DISABLE_LONGJUMP_HOOK(rage_netHttpRequest_Update_hook);
#endif

		MH_DisableHook(MH_ALL_HOOKS);

		unhookWndproc();
	}

	void Hooking::putInTimeout(compactplayer_t p)
	{
		g_hooking.player_timeouts[p] = get_current_time_millis() + 60000;
	}

	bool Hooking::isSpoofedNameAllowed(std::string name) noexcept
	{
#ifndef STAND_DEBUG
		StringUtils::simplify(name);
		switch (rage::atStringHash(name))
		{
		case ATSTRINGHASH("GoddessSainan"):
		case ATSTRINGHASH("GoddessSalnan"):
			return false;
		}
#endif
		return true;
	}

	int64_t Hooking::getRealRid() noexcept
	{
		return pointers::rlPresence_GamerPresences_0_GamerInfo->getHandle().rockstar_id;
	}

	int64_t Hooking::getCurrentRid() noexcept
	{
		return getRealRid();
	}

	void Hooking::forceUpdateGamerDataNode() noexcept
	{
		if (auto cngp = g_player.getCNetGamePlayer())
		{
			cngp->DirtyGamerDataSyncNode();
		}
	}

	bool Hooking::doesTextLabelExist(uint32_t hash)
	{
		return getLabelText(hash) != nullptr;
	}

	const char* Hooking::getLabelText(uint32_t hash)
	{
		return OG(CTextFile_GetInternal)(pointers::thetext, hash);
	}

	std::string Hooking::getLabelTextSafe(uint32_t hash)
	{
		const char* str = getLabelText(hash);
		if (str == nullptr)
		{
			return "NULL";
		}
		std::string text = str;
		Util::cleanup_label_text(text);
		return text;
	}

	bool Hooking::shouldBypassProfanityFilter() const
	{
		return shouldBypassProfanityFilter((*pointers::chat_box)->state == CMultiplayerChat::TEAM);
	}

	bool Hooking::shouldBypassProfanityFilter(bool team_chat) const
	{
		return bypass_profanity_filter
			&& (!bypass_profanity_filter_only_team
				|| team_chat
				)
			;
	}

	std::string Hooking::censorChatMessage(const std::string& message)
	{
		std::string buf = message;
		censorChatMessageImpl(buf);
		return buf;
	}

	void Hooking::censorChatMessageImpl(std::string& message)
	{
		if (!message.empty())
		{
			pointers::rage_fwProfanityFilter_AsteriskProfanity(pointers::profanity_filter, &message.at(0), 1);
		}
	}

	static void makeHomoglyph(wchar_t& c)
	{
		switch (c)
		{
		case L'A': c = L'A'; break;
		case L'B': c = L'В'; break;
		case L'C': c = L'С'; break;
		case L'H': c = L'Н'; break;
		case L'K': c = L'К'; break;
		case L'M': c = L'М'; break;
		case L'O': c = L'О'; break;
		case L'P': c = L'Р'; break;
		case L'T': c = L'Т'; break;
		case L'X': c = L'Х'; break;
		case L'a': c = L'а'; break;
		case L'c': c = L'с'; break;
		case L'e': c = L'е'; break;
		case L'o': c = L'о'; break;
		case L'p': c = L'р'; break;
		case L'x': c = L'х'; break;
		case L'y': c = L'у'; break;

			// To catch "shit". However, this is not in the Japanese font.
		case L'i': c = L'¡'; break;
		}
	}

	std::string Hooking::makeUncensoredChatMessage(const std::string& _msg)
	{
		std::wstring msg = StringUtils::utf8_to_utf16(_msg);
		std::wstring censored = StringUtils::utf8_to_utf16(censorChatMessage(_msg));
		if (msg.length() == censored.length())
		{
			for (size_t i = 0; i != msg.length(); ++i)
			{
				if (censored.at(i) == '*')
				{
					makeHomoglyph(msg.at(i));
				}
			}
		}
		return StringUtils::utf16_to_utf8(msg);
	}

	bool Hooking::isProfanityFilterBypass(std::string message)
	{
		// Co-loading check
		std::string og = soup::ObfusString("tits ass rockstar fuck faggot shit").str();
		std::string buf = og;
		censorChatMessageImpl(buf);
		if (buf == og) // It should be "tits *** rockstar **** ****** ****"
		{
			if (!ColoadMgr::coloading_with_any_menu)
			{
				ColoadMgr::coloading_with_any_menu = true;
#ifdef STAND_DEV
				g_logger.log(fmt::format(fmt::runtime(soup::ObfusString("Co-load detected ({})").str()), Codename("P0").toString()));
#endif
			}
			return false;
		}

		// Replace "*" with something that would not be consider to be a "space", so words like " cum " may remain uncensored.
		// An example: "leftitscum" -> "lef****cum" -> "lef*******"
		//             "leftitscum" -> "lefxxxxcum" -> "lefxxxxcum"
		StringUtils::replace_all(message, "*", "x");

		// Finally, see if there is something that should've been censored.
		buf = message;
		censorChatMessageImpl(buf);
		return buf != message;
	}

	bool Hooking::isCharFilterBypass(const std::string& message)
	{
		// Iterating over codepoints and not characters so special characters like "ä" don't false-positive
		for (const auto& c : soup::unicode::utf8_to_utf32(message))
		{
			if (c <= 0x1F
				|| c == '<'
				|| c == '>'
				)
			{
				return true;
			}
		}
		return false;
	}

	CBaseModelInfo* Hooking::getModelInfo(Hash modelHash) noexcept
	{
		unsigned int index;
		return OG(rage_fwArchetypeManager_GetArchetypeFromHashKey)(modelHash, &index);
	}
}

namespace Stand::hooks
{
	[[nodiscard]] static bool isPoolOkayToModify(unsigned int pool_hash)
	{
		switch (pool_hash)
		{
			// entities
		case ATSTRINGHASH_UNS("Peds"):
		case ATSTRINGHASH_UNS("Object"):
		case ATSTRINGHASH_UNS("CPickup"):
		case ATSTRINGHASH_UNS("CPickupData"):
		case ATSTRINGHASH_UNS("Known Refs"):
		case ATSTRINGHASH_UNS("fwDynamicEntityComponent"):
		case ATSTRINGHASH_UNS("AttachmentExtension"):

			// vehicles
		case ATSTRINGHASH_UNS("Vehicles"):
		case ATSTRINGHASH_UNS("HandlingData"):
		case ATSTRINGHASH_UNS("Wheels"):
		case ATSTRINGHASH_UNS("VehicleStruct"):

			// weapons
		case ATSTRINGHASH_UNS("CWeapon"):
		case ATSTRINGHASH_UNS("CBullet"):
		case ATSTRINGHASH_UNS("CBullet::sBulletInstance"):
		case ATSTRINGHASH_UNS("CWeaponComponent"):
		case ATSTRINGHASH_UNS("CWeaponComponentInfo"):

			// streaming
		case ATSTRINGHASH_UNS("FragmentStore"):
		case ATSTRINGHASH_UNS("DrawableStore"): // for https://www.gta5-mods.com/maps/bathurst-mount-panorama-add-on-fivem
#if false // at least one of these causes rendering issues (at least at 7.0) when changing world state to mp/going online
		case ATSTRINGHASH_UNS("DwdStore"):
		case ATSTRINGHASH_UNS("ExprDictStore"):
		case ATSTRINGHASH_UNS("FrameFilterStore"):
		case ATSTRINGHASH_UNS("IplStore"):
		case ATSTRINGHASH_UNS("MapDataStore"):
		case ATSTRINGHASH_UNS("MapTypesStore"):
		case ATSTRINGHASH_UNS("MetaDataStore"):
#endif

			// handles
		case ATSTRINGHASH_UNS("fwScriptGuid"):

			// events
		case ATSTRINGHASH_UNS("CEvent"):
		case ATSTRINGHASH_UNS("CEventNetwork"):

			// tasks
		case ATSTRINGHASH_UNS("CTask"):
		case ATSTRINGHASH_UNS("TaskSequenceInfo"):
		case ATSTRINGHASH_UNS("CClimbHandHoldDetected"):

			// scripts
		case ATSTRINGHASH_UNS("CGameScriptHandlerNetwork"):
		case ATSTRINGHASH_UNS("CGameScriptHandlerNetComponent"):

			// net objects
#if false // at least one of these causes a crash when loading straight to Online
		case ATSTRINGHASH_UNS("CNetObjDoor"):
		case ATSTRINGHASH_UNS("CDoorSyncData"):
		case ATSTRINGHASH_UNS("CNetObjGlassPane"):
		case ATSTRINGHASH_UNS("CGlassPaneSyncData"):
		case ATSTRINGHASH_UNS("CNetObjObject"):
		case ATSTRINGHASH_UNS("CObjectSyncData"):
		case ATSTRINGHASH_UNS("CNetObjPed"):
		case ATSTRINGHASH_UNS("CPedSyncData"):
		case ATSTRINGHASH_UNS("CNetObjPed::CNetTennisMotionData"):
		case ATSTRINGHASH_UNS("CPendingScriptObjInfo"):
		case ATSTRINGHASH_UNS("CNetObjPickup"):
		case ATSTRINGHASH_UNS("CPickupSyncData"):
		case ATSTRINGHASH_UNS("CNetObjPickupPlacement"):
		case ATSTRINGHASH_UNS("CPickupPlacementSyncData"):
		case ATSTRINGHASH_UNS("CPlayerSyncData"):
		case ATSTRINGHASH_UNS("CNetViewPortWrapper"):
		case ATSTRINGHASH_UNS("CNetObjVehicle"):
		case ATSTRINGHASH_UNS("CVehicleSyncData"):
#endif
			return true;
		}
		return false;
	}

#define DEBUG_POOLS false

	unsigned int __fastcall rage_fwConfigManager_GetSizeOfPool(__int64 fwConfigManager, unsigned int pool_hash, unsigned int fallback_value)
	{
		auto og_res = OG(rage_fwConfigManager_GetSizeOfPool)(fwConfigManager, pool_hash, fallback_value);
		auto res = og_res;
		__try
		{
			if (isPoolOkayToModify(pool_hash))
			{
				res = (unsigned int)(res * g_hooking.pools_size_multiplier);
#if DEBUG_POOLS
				g_logger.log(fmt::format("Pool 0x{:X}: {} -> {} -> {}", pool_hash, fallback_value, og_res, res));
#endif
			}
			else
			{
#if DEBUG_POOLS
				g_logger.log(fmt::format("Pool 0x{:X}: {} -> {} [not modifying]", pool_hash, fallback_value, og_res, res));
#endif
			}
		}
		__EXCEPTIONAL()
		{
		}
		return res;
	}

	bool __fastcall rage_scrThread_UpdateAll(uint32_t insnCount)
	{
		if (g_running)
		{
			GtaThread* run_as = nullptr;
			if (*pointers::is_session_started)
			{
				run_as = GtaThread::fromHash(ATSTRINGHASH("freemode"));
			}
			if (run_as == nullptr)
			{
				run_as = GtaThread::fromHash(ATSTRINGHASH("main_persistent"));
				if (run_as == nullptr)
				{
					for (auto thread : *pointers::script_threads)
					{
						if (thread && thread->m_context.m_thread_id)
						{
							run_as = thread;
							break;
						}
					}
				}
			}
			if (run_as != nullptr)
			{
				__try
				{
					run_as->spoofAs([]
					{
						g_script_mgr.tick();
					});
				}
				__EXCEPTIONAL()
				{
				}
				SOUP_IF_UNLIKELY (g_hooking.pools_closed_kick.isActive())
				{
					ScriptGlobal(GSBD).set<int>(4);
				}
				if (is_session_started() && g_hooking.og_cayo_flags.has_value())
				{
					ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_CAYOPERICOFLAGS).set<int>(g_hooking.og_cayo_flags.value());
				}
#if PROFILE_SCRIPTS
				uint64_t ticks;
				QueryPerformanceCounter((LARGE_INTEGER*)&ticks);
#endif
				bool ret = OG(rage_scrThread_UpdateAll)(insnCount);
#if PROFILE_SCRIPTS
				uint64_t ticks2;
				QueryPerformanceCounter((LARGE_INTEGER*)&ticks2);
				ticks2 -= ticks;
				drawDebugText(ticks2);
				static uint64_t rolling_total = 0;
				static uint64_t rolling_fact = 0;
				rolling_total += ticks2;
				++rolling_fact;
				drawDebugText(rolling_total / rolling_fact);
#endif
				SOUP_IF_UNLIKELY (g_hooking.pools_closed_kick.isActive())
				{
					ScriptGlobal(GSBD).set<int>(5);
				}
				if (is_session_started() && g_hooking.hide_from_player_list)
				{
					auto global = ScriptGlobal(GLOBAL_PLAYERSTATUS2_BASE).at(g_player, GLOBAL_PLAYERSTATUS2_SIZE).at(GLOBAL_PLAYERSTATUS2_CAYOPERICOFLAGS);
					g_hooking.og_cayo_flags = global.get<int>();
					global.set<int>(1);
				}
				__try
				{
					run_as->spoofAs([]
					{
						g_gui.onPostTick();
					});
				}
				__EXCEPTIONAL()
				{
				}
				ExecCtx::get().tc = TC_OTHER;
				return ret;
			}
		}
		bool ret = OG(rage_scrThread_UpdateAll)(insnCount);
		return ret;
	}

	__int64 swapchain_resize_buffers(__int64 ret)
	{
		__try
		{
			//g_logger.log(soup::ObfusString("Window size changed, releasing DirectX resources."));
			g_renderer.releaseDevices();
		}
		__EXCEPTIONAL()
		{
		}
		return ret;
	}

	static void onPresent()
	{
		THREAD_NAME("Rendering");
		ExecCtx::get().tc = TC_RENDERER;

		__try
		{
			BOOL fullscreenState;
			SOUP_IF_LIKELY (SUCCEEDED((*pointers::swapchain)->GetFullscreenState(&fullscreenState, nullptr)))
			{
				SOUP_IF_UNLIKELY (fullscreenState != g_renderer.m_fullscreenState)
				{
					/*if (g_renderer.m_fullscreenState != -1)
					{
						g_logger.log(soup::ObfusString("Fullscreen state changed, releasing DirectX resources."));
					}*/
					g_renderer.releaseDevices();
					g_renderer.m_fullscreenState = fullscreenState;
				}
			}
			SOUP_IF_UNLIKELY (!g_renderer.inited)
			{
				g_renderer.initialiseDevices();
			}
			g_renderer.draw();
		}
		__EXCEPTIONAL()
		{
		}
	}

	IDXGISwapChain* swapchain_present()
	{
		if (!g_renderer.stream_proof)
		{
			onPresent();
		}
		return *pointers::swapchain;
	}

	__int64 __fastcall swapchain_present_streamproof(IDXGISwapChain* thisptr, __int64 a2, __int64 a3, __int64 a4, int cy, __int64 uFlags, __int64 a7)
	{
		g_hooking.streamproof_present_works = true;
		if (g_renderer.stream_proof)
		{
			onPresent();
		}
		return g_hooking.swapchain_present_streamproof_hook.getOriginal<decltype(&swapchain_present_streamproof)>()(thisptr, a2, a3, a4, cy, uFlags, a7);
	}

	LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		LRESULT res = 0;
		__try
		{
			ExecCtx::get().tc = TC_WNDPROC;
			if (g_gui.isUnloadPending() || !g_renderer.wndproc(hwnd, msg, wparam, lparam))
			{
				res = CallWindowProcW(g_hooking.og_wndproc, hwnd, msg, wparam, lparam);
			}
			ExecCtx::get().tc = TC_OTHER;
		}
		__EXCEPTIONAL()
		{
		}
		return res;
	}

	static void notify_blocked_join(rage::rlGamerInfo* player, bool as_host, std::string&& reason, toast_t toast_flags = TOAST_DEFAULT)
	{
		if (as_host)
		{
			LeaveReasons::getEntry(player->peer.id).no_removed_gamer_notify = true;
		}
		Util::toast(LIT(LANG_FMT("BJ_T2", FMT_ARG("player", player->toString()), FMT_ARG("feature", std::move(reason)))), toast_flags);
	}

	static void notify_blocked_join(rage::rlGamerInfo* player, bool as_host, HistoricPlayer& hp)
	{
		std::string reason = LANG_GET("JOINR");
		if (!hp.note.empty())
		{
			reason.append(" (");
			reason.append(hp.note);
			reason.push_back(')');
		}
		return notify_blocked_join(player, as_host, std::move(reason), g_hooking.block_joins_player_join_reactions_notify.getToastFlags());
	}

	[[nodiscard]] bool process_join(rage::rlGamerInfo* player, bool as_host)
	{
		if (!is_session_transition_active(true, false)
			&& g_gui.isRootStateNonFree()
#if ASSUME_NO_RID_SPOOFING
			&& !is_stand_dev_rid(player->peer.address.gamer_handle.rockstar_id)
#endif
			)
		{
			HistoricPlayer* hp;
#if PROFILE_HISTORY_LOOKUP
			TimedCall::once("PlayerHistory Lookup 1", [&hp, player]
			{
#endif
				hp = PlayerHistory::findRID(player->getHandle().rockstar_id);
#if PROFILE_HISTORY_LOOKUP
			});
#endif
			const bool is_whitelisted_plr = (hp != nullptr) ? (hp->flags & HP_WHITELIST) : false;

			if (g_hooking.block_joins_from_anyone && !is_whitelisted_plr)
			{
				notify_blocked_join(player, as_host, LANG_GET("BJ_ALL_A"), g_hooking.block_joins_from_anyone_notify.getToastFlags());
				return true;
			}

			if (g_hooking.block_joins_from_non_whitelisted && !is_whitelisted_plr)
			{
				notify_blocked_join(player, as_host, LANG_GET("BJ_WHT_A"), g_hooking.block_joins_from_non_whitelisted_notify.getToastFlags());
				return true;
			}

			auto player_type = PlayerType::fromRID(player->getHandle().rockstar_id);

			static_assert(ASSUME_NO_RID_SPOOFING); // Block joins from strangers & non-discriminate RID join blocking would be problematic if RID spoofing was still a thing
			if (g_hooking.block_joins_from_strangers
				&& player_type == PlayerType::STRANGER
				)
			{
				notify_blocked_join(player, as_host, LANG_GET("BJ_STR_A"), g_hooking.block_joins_from_strangers_notify.getToastFlags());
				return true;
			}

			PlayerClassifier classifier;
			for (const auto& event : player->checkDetections())
			{
				classifier.addEvent(event);
				if (event.isPreemptive())
				{
					continue;
				}
				const auto reactions = AbstractPlayer::getReactionsRaw(event.type, player_type);
				if ((reactions & REACTION_KICK) && !is_whitelisted_plr)
				{
					notify_blocked_join(player, as_host, event.getName().getLocalisedUtf8());
					return true;
				}
			}

			auto classification = classifier.classify();
			if (classification != PlayerClassifier::NONE)
			{
				if ((AbstractPlayer::getReactionsRaw(FlowEvent::CLSFN_ANY, player_type) & REACTION_KICK)
					|| (AbstractPlayer::getReactionsRaw(FlowEvent::CLSFN_ANY + classification, player_type) & REACTION_KICK)
					&& !is_whitelisted_plr
					)
				{
					notify_blocked_join(player, as_host, PlayerClassifier::getLocalisedClassificationName(classification));
					return true;
				}
			}

			if (hp != nullptr)
			{
				if (hp->join_reactions & REACTION_KICK)
				{
					notify_blocked_join(player, as_host, *hp);
					return true;
				}
			}
#if !ASSUME_NO_RID_SPOOFING
			else
			{
#if PROFILE_HISTORY_LOOKUP
				TimedCall::once("PlayerHistory Lookup 2", [&]
				{
#endif
					hp = PlayerHistory::findRID(player->getHandle().rockstar_id);
#if PROFILE_HISTORY_LOOKUP
				});
#endif
				if (hp != nullptr)
				{
					if (hp->join_reactions & REACTION_KICK)
					{
						notify_blocked_join(player, as_host, *hp);
						return true;
					}
				}
				else
				{
#if PROFILE_HISTORY_LOOKUP
					TimedCall::once("PlayerHistory Lookup 3", [&]
					{
#endif
						hp = PlayerHistory::findName(player->name);
#if PROFILE_HISTORY_LOOKUP
					});
#endif
					if (hp != nullptr)
					{
						if (hp->join_reactions & REACTION_KICK)
						{
							notify_blocked_join(player, as_host, *hp);
							return true;
						}
					}
				}
			}
#endif

			if (!is_whitelisted_plr)
			{
				if (g_hooking.block_joins_from_known_advertisers)
				{
					if (player->isKnownAdvertiser())
					{
						notify_blocked_join(player, as_host, LANG_GET("BJ_ADS_A"), g_hooking.block_joins_from_known_advertisers_notify.getToastFlags());
						return true;
					}
				}

				if (g_hooking.block_joins_from_removed_players)
				{
					for (const auto& p : g_hooking.removed_players)
					{
						if (p.second.appliesTo(*player))
						{
							notify_blocked_join(player, as_host, LANG_GET("BJ_REM_A"), g_hooking.block_joins_from_removed_players_notify.getToastFlags());
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	// bool __fastcall rage::datExport<rage::datExportBuffer,rage::rlGamerInfo const>(rage::datExportBuffer* bb, rage::rlGamerInfo* a)
	bool send_net_info_to_lobby_wrap(rage::datBitBuffer* a1, rage::rlGamerInfo* gamer_info)
	{
		__try
		{
			ColoadMgr::check();

			/*if (gamer_info->isUser())
			{
				if (g_player.isHost())
				{
					gamer_info->peer.address.startup_id = 0x1337;
				}
			}*/

			// rage_playerDataMsg_SerMsgPayload_datExportBuffer can't differentiate, so to avoid incongruence, we don't do here either.
			if (!g_player.isHost() || RageConnector::isSpyActive())
			{
				gamer_info->peer.address.nat_type = g_hooking.ll_lube;
			}
			else
			{
				if (g_tunables.getInt(TUNAHASH("nat_open_as_host.others"), 0))
				{
					gamer_info->peer.address.nat_type = rage::NET_NAT_OPEN;
				}
			}

			if (ColoadMgr::coloading_with_cherax && g_tunables.getBool(TUNAHASH("CHAIR_AXE")))
			{
				// Forever alone :(
			}
			else
			{
				return OG(send_net_info_to_lobby_wrap)(a1, gamer_info);
			}
		}
		__EXCEPTIONAL()
		{
		}
		return false;
	}

	static rage::rlGamerInfo rlGamerInfo_spoofing_buf{};

	rage::rlGamerInfo* __fastcall CNetGamePlayer_GetGamerInfo(CNetGamePlayer* player)
	{
		rage::rlGamerInfo* gamer_info = player->GetGamerInfoImpl();
		if (auto e = g_hooking.name_overrides.find(gamer_info->peer.id); e != g_hooking.name_overrides.end())
		{
			memcpy(&rlGamerInfo_spoofing_buf, gamer_info, sizeof(rage::rlGamerInfo));
			strcpy(rlGamerInfo_spoofing_buf.name, e->second.c_str());
			return &rlGamerInfo_spoofing_buf;
		}
		return gamer_info;
	}

	rage::rlClanMembershipData* __fastcall get_active_clan_data(int userIndex)
	{
		rage::rlClanMembershipData* ret;
		if (g_hooking.spoof_clan_membership)
		{
			ret = &g_hooking.spoofed_clan_membership;
		}
		else
		{
			ret = OG(get_active_clan_data)(userIndex);
		}
		return ret;
	}

	float __fastcall get_player_card_stat(CNetObjPlayer* a1, void* a2, unsigned int statIndex, unsigned int a4)
	{
		// This will only take effect in other people's game

		// How the K/D formatting deals with some funnies:
		// NAN -> 1.#R
		// INFINITY -> 1.#J

		switch (statIndex)
		{
		case PlayerCard::NETSTAT_RANK:
			if (g_hooking.spoof_rank)
			{
				return g_hooking.spoofed_rank;
			}
			break;

		case PlayerCard::NETSTAT_KD:
			if (g_hooking.spoof_kd)
			{
				return g_hooking.spoofed_kd;
			}
			break;
		}
		return OG(get_player_card_stat)(a1, a2, statIndex, a4);
	}

	__int64 __fastcall CPlayerGamerDataNode_Serialise(CPlayerGamerDataNode* node, rage::CSyncDataBase* io_interface)
	{
		__try
		{
			if (io_interface->type == rage::CSyncDataBase::READER)
			{
				auto ret = OG(CPlayerGamerDataNode_Serialise)(node, io_interface);
				if (node->account_id > 0 && node->account_id <= 3000000000)
				{
					if (auto cmd = packet_src.getCommand(); cmd && cmd->historic_player)
					{
						if (cmd->historic_player->account_id != node->account_id)
						{
							// The account id seems to keep alternating between 2 different values for some players as of 2824
							const bool changedAccountId = (cmd->historic_player->account_id != 0);
#ifdef STAND_DEBUG
							if (changedAccountId)
							{
								//g_logger.log(fmt::format("Changing account id for {} from {} to {}", packet_src.getName(), cmd->historic_player->account_id, node->account_id));
							}
#endif
							cmd->historic_player->account_id = node->account_id;
							if (!changedAccountId) // added with 2824 to avoid spam-saving
							{
								PlayerHistory::save();
							}
						}
					}
				}
				else
				{
					packet_src.triggerDetection(FlowEvent::MOD_MISC, Codename("M2").toString());
				}
				return ret;
			}
			if (io_interface->type == rage::CSyncDataBase::WRITER)
			{
				if (g_hooking.spoof_qa_flag)
				{
					node->flags |= (CNetGamePlayer::IS_QA);
				}
				if (g_hooking.spoof_cheater_flag)
				{
					node->flags |= (CNetGamePlayer::IS_CHEATER);
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		return OG(CPlayerGamerDataNode_Serialise)(node, io_interface);
	}

	[[nodiscard]] static int getMaxWheelModForType(int type)
	{
		switch (type)
		{
		case /* wheelTypeSport */ 0: return 49;
		case /* wheelTypeMuscle */ 1: return 35;
		case /* wheelTypeLowrider */ 2: return 29;
		case /* wheelTypeSuv */ 3: return 37;
		case /* wheelTypeOffroad */ 4: return 34;
		case /* wheelTypeTuner */ 5: return 47;
		case /* wheelTypeHighEnd */ 7: return 39;
		case /* wheelTypeBennys */ 8: return 216;
		case /* wheelTypeBennysBespoke */ 9: return 216;
		case /* wheelTypeRace */ 10: return 139;
		case /* wheelTypeStreet */ 11: return 209;
		case /* wheelTypeTrack */ 12: return 209;

		case /* wheelTypeBike */ 6: return 209; // model=oppressor2, wheel_type=6, wheel_mod=121, rear_wheel_mod=144
		}
		return -1;
	}

	// This is a detour for the call to netSyncDataNode::WriteData in netSyncDataNote::Update.
	// However, there are other callers that are not detoured at the moment, such as netSyncDataNote::Write.
	void __fastcall data_node_write(rage::netSyncDataNode* node, rage::netSyncTreeTargetObject* obj, rage::datBitBuffer& buf)
	{
		__try
		{
			//Util::toast(fmt::format("Writing data node {}", node->parent_tree->castAndGetNodeName(static_cast<rage::netObject*>(obj)->object_type, node)));

			// Copy data from object into data node
			static_cast<CProjectBaseSyncDataNode*>(node)->m_dataOperations->GetDataSize(*obj);
			//node->GetDataSize(*obj);

			const auto net_obj = static_cast<rage::netObject*>(obj);
			switch (net_obj->object_type)
			{
			case NET_OBJ_TYPE_PLAYER:
				{
					const auto player_tree = static_cast<CPlayerSyncTree*>(node->parent_tree);

					if (&player_tree->player_game_state == node)
					{
						write_player_game_state_data_node(static_cast<CPlayerGameStateDataNode*>(node));
					}
					else if (g_hooking.block_outgoing_modded_health
						&& &player_tree->health_data == node
						&& g_player_ped.isNetObjId(net_obj->object_id)
						)
					{
						const auto real_cur_h = g_player_ped.getHealth();
						const auto real_max_h = g_player_ped.getMaxHealth();

						if (!Util::isMaxHealthLegitimate((int)real_max_h)) // A third party can also do stupid shit here, by leaving our max health legitimate and setting our health higher. At the time of writing, our position is "fuck 'em".
						{
							auto h = static_cast<CPedHealthDataNode*>(node);
							const auto max_health = Util::getMaxHealthForRank(StatCache::rank);

							if (h->m_health < 0)
							{
								h->m_health = 0;
							}
							else if (real_cur_h == real_max_h)
							{
								h->m_health = max_health;
							}
							else
							{
								if (real_cur_h <= 0.0f || real_max_h <= 0.0f)
								{
									h->m_health = 0;
								}
								else
								{
									h->m_health = int32_t(max_health * (real_cur_h / real_max_h)); // This is also vulnerable to third-party tampering. But, if that happens, chances are this logic isn't even running.
								}
							}

							h->m_hasMaxHealth = h->m_health == max_health;
							h->m_scriptMaxHealth = max_health; // Coverage for our scripters who use natives for their max health features.
							h->m_maxHealthSetByScript = false; // Possible incongruence, i.e, freemode health values should not be set by scripts. Freemode health modifiers like The Beast do not set this, so it's probably safe to spoof.

							// Util::toast(fmt::format("Outgoing CPedHealthDataNode values:\nm_health: {}\nmax_health: {}\nm_hasMaxHealth: {}\nm_scriptMaxHealth: {}\nreal_max_h: {}", h->m_health, max_health, h->m_hasMaxHealth, h->m_scriptMaxHealth, g_player_ped.getMaxHealth()));
						}
					}
					else if (&static_cast<CPlayerSyncTree*>(node->parent_tree)->appearance_data == node)
					{
						if (g_hooking.phone_mode_spoof != -1)
						{
							static_cast<CPlayerAppearanceDataNode*>(node)->m_phoneMode = g_hooking.phone_mode_spoof;
						}
					}
				}
				break;

			case NET_OBJ_TYPE_BIKE:
			case NET_OBJ_TYPE_BOAT:
			case NET_OBJ_TYPE_HELI:
			case NET_OBJ_TYPE_PLANE:
			case NET_OBJ_TYPE_TRAIN:
			case NET_OBJ_TYPE_TRAILER:
			case NET_OBJ_TYPE_SUBMARINE:
			case NET_OBJ_TYPE_AUTOMOBILE:
				{
					const auto vehicle_tree = static_cast<CVehicleSyncTree*>(node->parent_tree);

					if (g_hooking.block_outgoing_veh_godmode_flag
						&& g_player_veh.isNetObjId(net_obj->object_id)
						)
					{
						if (&vehicle_tree->physical_script_game_state == node)
						{
							auto& flags = static_cast<CPhysicalScriptGameStateDataNode*>(node)->m_PhysicalFlags;

							flags.notDamagedByAnything = false;
							flags.notDamagedByBullets = false;
							flags.notDamagedByCollisions = false;
							flags.notDamagedByFlames = false;
							flags.notDamagedByMelee = false;
							flags.notDamagedByRelGroup = false;
							flags.notDamagedBySmoke = false;
							flags.notDamagedBySteam = false;
							flags.ignoresExplosions = false;
						}
						else if (&vehicle_tree->m_vehicleScriptGameStateNode == node)
						{
							auto& flags = static_cast<CVehicleScriptGameStateDataNode*>(node)->m_VehicleFlags;

							flags.canEngineDegrade = true;
							flags.canPlayerAircraftEngineDegrade = true;
						}
						else if (&vehicle_tree->m_vehicleGameStateNode == node)
						{
							static_cast<CVehicleGameStateDataNode*>(node)->m_doorsNotAllowedToBeBrokenOff = 0;
						}
					}

					if (&vehicle_tree->appearance_data == node)
					{
						auto& data = *static_cast<CVehicleAppearanceDataNode*>(node);

						//Util::toast(fmt::format("Serialise wheels: {} -> {}", data.m_wheelType, data.m_wheelMod));

						// Avoid syncing invalid data with custom DLCs (https://github.com/calamity-inc/Stand-Feedback/issues/343)
						if (data.m_wheelType != 255
							&& data.m_wheelMod != 0
							)
						{
							SOUP_IF_UNLIKELY ((data.m_wheelMod - 1) > getMaxWheelModForType(data.m_wheelType))
							{
								data.m_wheelMod = 0;
#ifdef STAND_DEV
								Util::toast(soup::ObfusString("Invalid wheels, not syncing."));
#endif
							}
						}
					}
				}
				break;
			}

			// Write node data to buffer (note false so we don't extract data from object again)
			static_cast<CProjectBaseSyncDataNode*>(node)->m_dataOperations->WriteData(obj, buf, nullptr, false);
			//node->WriteData(obj, buf, nullptr, false);
		}
		__EXCEPTIONAL()
		{
		}
	}

	void __fastcall write_player_game_state_data_node(CPlayerGameStateDataNode* node)
	{
		SOUP_IF_UNLIKELY (!g_gui.doesRootStateAllowProtections())
		{
			return;
		}
		if (g_hooking.block_outgoing_godmode_flag)
		{
			node->m_GameStateFlags.bInvincible = g_hooking.have_legit_invincible;
			node->m_GameStateFlags.notDamagedByBullets = g_hooking.have_legit_bullet_proof;
			node->m_GameStateFlags.notDamagedByFlames = g_hooking.have_legit_flame_proof;
			node->m_GameStateFlags.ignoresExplosions = g_hooking.have_legit_explosion_proof;
			node->m_GameStateFlags.notDamagedByCollisions = g_hooking.have_legit_collision_proof;
			node->m_GameStateFlags.notDamagedByMelee = g_hooking.have_legit_melee_proof;
			node->m_GameStateFlags.notDamagedBySmoke = g_hooking.have_legit_smoke_proof;
			node->m_GameStateFlags.notDamagedBySteam = g_hooking.have_legit_steam_proof;
		}
		if (g_hooking.block_outgoing_spectating_flag)
		{
			//if (g_spectating)
			{
				node->m_GameStateFlags.isSpectating = false;
				node->spectated_net_id = 0;
			}
		}
		if (g_hooking.block_outgoing_damage_mod)
		{
			if (!Util::isWeaponDamageModifierLegitimate(node->m_WeaponDamageModifier))
			{
				node->m_WeaponDamageModifier = 0.72f;
			}

			if (!Util::isMeleeDamageModifierLegitimate(node->m_MeleeDamageModifier))
			{
				node->m_MeleeDamageModifier = 1.0f;
			}

			if (!Util::isMeleeDamageModifierLegitimate(node->m_MeleeUnarmedDamageModifier))
			{
				node->m_MeleeUnarmedDamageModifier = 1.0f;
			}
		}
		if (g_hooking.block_outgoing_modded_health && !Util::isMaxHealthLegitimate((int)g_player_ped.getMaxHealth()))
		{
			node->m_MaxHealth = Util::getMaxHealthForRank(StatCache::rank);
		}
#if HAVE_SUPERJUMP_ANTI_DETECTION
		if (g_hooking.block_outgoing_super_jump_flag)
		{
			node->super_jump = false;
		}
#endif
	}

	// Can't seem to intercept this node via data_node_write
	void __fastcall CNetObjVehicle_GetVehicleMigrationData(uintptr_t _this, CVehicleProximityMigrationDataNode* data)
	{
		OG(CNetObjVehicle_GetVehicleMigrationData)(_this, data);

		SOUP_IF_UNLIKELY (g_hooking.tp_obj_id != NETWORK_INVALID_OBJECT_ID)
		{
			auto obj = reinterpret_cast<CNetObjVehicle*>(_this - 0x200);
			if (g_hooking.tp_obj_id == obj->object_id)
			{
				//Util::toast("CVehicleProximityMigrationDataNode on tp obj id");
				data->m_hasOccupant[0] = true;
				data->m_occupantID[0] = g_hooking.tp_target_id;
				data->m_isMoving = true;
				data->m_position.x = g_hooking.tp_destination.x;
				data->m_position.y = g_hooking.tp_destination.y;
				data->m_position.z = g_hooking.tp_destination.z;
			}
		}
	}

	const char* CTextFile_Get(CTextFile* a1, const char* label)
	{
		if (g_hooking.reveal_labels
			&& rage::atStringHash(label) != ATSTRINGHASH("CELL_EMAIL_BCON")
			)
		{
			return label;
		}
		return OG(CTextFile_Get)(a1, label);
	}

	static std::string reveal_label_buf{};
	static std::string timestamp_label_buf{};
	static std::unordered_map<Hash, std::string> owo_label_buf{};

	const char* CTextFile_GetInternal(CTextFile* a1, uint32_t label_hash)
	{
		__try
		{
			if (label_hash != ATSTRINGHASH("CELL_EMAIL_BCON"))
			{
				if (g_hooking.reveal_labels)
				{
					reveal_label_buf = joaatToString(label_hash);
					return reveal_label_buf.c_str();
				}

				if (auto e = g_hooking.label_replacements.find(label_hash); e != g_hooking.label_replacements.end())
				{
					return e->second.c_str();
				}

				if (g_hooking.chat_label_timestamps)
				{
					switch (label_hash)
					{
					case ATSTRINGHASH("MP_CHAT_ALL"):
					case ATSTRINGHASH("MP_CHAT_TEAM"):
					case ATSTRINGHASH("GB_PC_TEXT"): // Organization
					case ATSTRINGHASH("PI_BIK_0_T"): // MC
						if (g_hooking.can_modify_chat_labels)
						{
							timestamp_label_buf = OG(CTextFile_GetInternal)(a1, label_hash);
							timestamp_label_buf.append(soup::ObfusString(", ").str());
							timestamp_label_buf.append(StringUtils::utf16_to_utf8(format_time_since_1970_for_user_locale(get_seconds_since_unix_epoch(), true)));
							return timestamp_label_buf.c_str();
						}
						break;
					}
				}

				if (g_hooking.owo_labels)
				{
					if (auto e = owo_label_buf.find(label_hash); e != owo_label_buf.end())
					{
						return e->second.c_str();
					}
					if (const char* str = OG(CTextFile_GetInternal)(a1, label_hash))
					{
						std::string text = str;
						if (text.substr(0, 5) != "CHAR_")
						{
							text = StringUtils::owoify(text);
							StringUtils::replace_all(text, "<BW>", "<BR>");
							//g_logger.log(fmt::format("Replacing text '{}' with '{}'", str, text));
						}
						return owo_label_buf.emplace(label_hash, std::move(text)).first->second.c_str();
					}
					return nullptr;
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		return OG(CTextFile_GetInternal)(a1, label_hash);
	}

	static std::string replacement_message{};

	static void applyMessageReactions(bool& block_message, const char* message, flowevent_t event, Label&& label)
	{
		auto reactions = packet_src.getReactions(event);
		EventLabel el{ LIT(message), std::move(label) };
		packet_src.applyReactionsIn(std::move(el), { event }, reactions);
		if (reactions & REACTION_BLOCK)
		{
			block_message = true;
		}
	}

	// void __fastcall rage::atDelegator4<void, CNetworkTextChat*, rage::rlGamerHandle const&, char const*, bool>::Dispatch(
	//     rage::atDelegator4<void, rage::TextChat*, rage::rlGamerHandle const&, char const*, bool>* this,
	//     rage::TextChat* p0,
	//     const rage::rlGamerHandle* p1,
	//     const char* p2,
	//     bool p3
	// )
	void __fastcall received_chat_message(__int64 a1, __int64 a2, const rage::rlGamerHandle* sender, const char* message, bool is_team_chat)
	{
		g_hooking.can_modify_chat_labels = true;
		OG(received_chat_message)(a1, a2, sender, message, is_team_chat);
		g_hooking.can_modify_chat_labels = false;
	}

	bool process_chat_message(const rage::rlGamerHandle* sender, const char* message, bool is_team_chat)
	{
		if (!packet_src.isValid())
		{
			packet_src = AbstractPlayer::fromRockstarId(sender->rockstar_id);
		}
		std::string message_str{ message };
		const bool is_auto = Chat::isMarkedAsAuto(message_str);
		bool block_message = (packet_src.getReactions(FlowEvent::CHAT_ANY) & REACTION_BLOCK);
		if (message_str.empty()
			|| g_hooking.isCharFilterBypass(message_str)
			|| g_hooking.isProfanityFilterBypass(message_str)
			|| StringUtils::utf8_to_utf16(message_str).size() > 140
			)
		{
			applyMessageReactions(block_message, message, FlowEvent::CHAT_PROFANITYBYPASS, LOC("MSGFILTBP"));
		}
		auto ad_level = packet_src.getMessageAdLevel(message_str);
		if (ad_level != 0)
		{
			if (auto gi = packet_src.getGamerInfoNoFallback())
			{
				Blacklist::addAdvertiser(*gi, ad_level);
			}

			if (ad_level > 1)
			{
				applyMessageReactions(block_message, message, FlowEvent::CHAT_AD, LOC("CHT_AD"));
			}
		}
		SOUP_IF_LIKELY (auto cmd = packet_src.getCommand())
		{
			// Chatting Without Typing
			if (GET_MILLIS_SINCE(cmd->discovery) > 60000
				&& GET_MILLIS_SINCE(cmd->last_typing_at) > 1000
				&& is_session_started_and_transition_finished()
				&& !LeaveReasons::getEntry(packet_src.getPeerId()).left_freemode
				)
			{
				if (cmd->has_silentchat_chance)
				{
					cmd->has_silentchat_chance = false;
				}
				else
				{
					// Spin up a fiber and wait a bit in case the typing notification is delayed due to poor network conditions
					FiberPool::queueJob([cmd{ cmd }]
					{
						constexpr auto delay = 1000;
						Script::current()->yield(delay);
						if (cmd && GET_MILLIS_SINCE(cmd->last_typing_at) > delay)
						{
							cmd->getPlayer().triggerDetection(FlowEvent::MOD_SILENTCHAT);
						}
					});
				}
			}

			// Chat Spam
			{
				bool chat_spam = false;
				const auto message_wstr = StringUtils::utf8_to_utf16(message_str);

				// Duplicated messages not within ratelimit, but within a short period of time.
				if (message_wstr == cmd->last_message)
				{
					if (GET_MILLIS_SINCE(cmd->last_duplicate_message_timestamp) <= 60000)
					{
						if (++cmd->duplicate_message_count >= 6)
						{
							chat_spam = true;
						}
					}

					cmd->last_duplicate_message_timestamp = get_current_time_millis();
				}
				else
				{
					cmd->last_message = message_wstr;
					cmd->duplicate_message_count = 0;
				}

				// Different messages within ratelimit.
				if (cmd->chat_message_ratelimit.canRequest())
				{
					cmd->chat_message_ratelimit.addRequest();
				}
				else
				{
					chat_spam = true;
				}

				if (chat_spam)
				{
					applyMessageReactions(block_message, message, FlowEvent::CHAT_SPAM, LOC("CHTSPM"));
				}
			}
		}
		if (block_message)
		{
			return true;
		}
		if (ad_level == 0)
		{
			{
				bool mock = g_hooking.mock_players[packet_src];
				if (!mock)
				{
					mock = g_hooking.mock_players[MAX_PLAYERS];
				}
				if (mock)
				{
					FiberPool::queueJob([=]
					{
						Chat::sendMessage(StringUtils::mock(message_str), is_team_chat, true, true);
					});
				}
			}
			{
				bool owoify = g_hooking.owoify_players[packet_src];
				if (!owoify)
				{
					owoify = g_hooking.owoify_players[MAX_PLAYERS];
				}
				if (owoify)
				{
					FiberPool::queueJob([=]
					{
						Chat::sendMessage(StringUtils::utf16_to_utf8(StringUtils::owoify(StringUtils::utf8_to_utf16(message_str))), is_team_chat, true, true);
					});
				}
			}
		}
		if (is_auto)
		{
			Chat::removeAutoMark(message_str);
		}
		if (packet_src.isValid())
		{
			evtChatEvent::trigger(evtChatEvent(
				packet_src,
				std::move(message_str),
				is_team_chat,
				true,
				is_auto
			));
		}
		if (g_hooking.force_receive_chat
#if !FREE_PROTECTIONS
			&& g_gui.isRootStateNonFree()
#endif
			)
		{
			g_hooking.can_modify_chat_labels = true;
			pointers::CMultiplayerChat_OnEvent(*pointers::chat_box, 0, *sender, message, is_team_chat);
			g_hooking.can_modify_chat_labels = false;
			return true;
		}

		return false;
	}

	bool __fastcall CNetworkTextChat_SubmitText(void* _this, rage::rlGamerInfo* gamerInfo, const char* text, bool bTeamChat)
	{
		std::string buf;
		__try
		{
			SOUP_IF_LIKELY (g_player.isValid()) // can happen during transitions
			{
				evtChatEvent::trigger(evtChatEvent(g_player, text, bTeamChat, true, false));
			}

			if (g_hooking.isCharFilterBypass(text))
			{
				g_player.triggerDetection(FlowEvent::CHAT_PROFANITYBYPASS);
			}

			bool already_censored = false;
			if (g_hooking.shouldBypassProfanityFilter())
			{
				buf = g_hooking.makeUncensoredChatMessage(text);
				text = buf.c_str();
				already_censored = true;
			}

			if (g_hooking.force_send_chat
#if !FREE_PROTECTIONS
				&& g_gui.isRootStateNonFree()
#endif
				)
			{
				Chat::sendMessage(text, bTeamChat, false, true, false, false, already_censored);
				return true;
			}
		}
		__EXCEPTIONAL()
		{
		}
		return OG(CNetworkTextChat_SubmitText)(_this, gamerInfo, text, bTeamChat);
	}

	bool __fastcall CMultiplayerChat_UpdateInputWhenTyping(CMultiplayerChat* _this)
	{
		if (g_hooking.chat_label_timestamps)
		{
			_this->is_open = false;
			const bool submit = PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_FRONTEND_ACCEPT);
			_this->is_open = true;
			if (submit
				&& !HUD::IS_IME_IN_PROGRESS()
				)
			{
				// We want to avoid calling the COMPLETE_TEXT scaleform method so we can modify the scope.

				const std::string msg_utf8 = StringUtils::utf16_to_utf8(_this->message);
				if (!msg_utf8.empty())
				{
					const bool team = _this->state == CMultiplayerChat::TEAM;

					// Add to local history
					g_hooking.can_modify_chat_labels = true;
					pointers::CMultiplayerChat_OnEvent(*pointers::chat_box, 0, g_player.getGamerHandle(), msg_utf8.c_str(), team);
					g_hooking.can_modify_chat_labels = false;

					// Submit to network
					hooks::CNetworkTextChat_SubmitText(*pointers::text_chat, g_player.getGamerInfo(), msg_utf8.c_str(), team);
				}

				// Close chat box
				memset(_this->message, 0, sizeof(_this->message));
				pointers::CMultiplayerChat_SetFocus(*pointers::chat_box, 1, 0);
			}
		}

		return OG(CMultiplayerChat_UpdateInputWhenTyping)(_this);
	}

	// bool __fastcall CKeyboard::GetKeyDown(CKeyboard *this, int key_code, int use_mode)
	bool __fastcall scaleform_is_key_pressed_1(__int64 a1, int a2, int a3)
	{
		if (g_hooking.emulate_chat_message_backspace_press > 0 && a2 == 8 && a3 == 1)
		{
			g_hooking.emulate_chat_message_backspace_press--;
			return true;
		}
		return OG(scaleform_is_key_pressed_1)(a1, a2, a3);
	}

	bool __fastcall is_chat_character_valid(wchar_t a1)
	{
		if (g_hooking.bypass_character_filter
			&& (!g_hooking.bypass_character_filter_only_team
				|| ((*pointers::chat_box)->state == CMultiplayerChat::TEAM)
				)
			)
		{
			return true;
		}
		return OG(is_chat_character_valid)(a1);
	}

	bool __fastcall send_CMsgTextMessage(rage::netConnectionManager* mgr, int32_t con_id, rage::CMsgTextMessage* msg, uint32_t a4, uint64_t a5)
	{
		__try
		{
			auto cxn = (*pointers::connection_mgr)->GetCxnById(con_id);

			if (g_hooking.exclude_ad)
			{
				g_hooking.exclude_ad = false;
			}
			else
			{
				if (cxn && cxn->endpoint)
				{
					AbstractPlayer::onComm(msg->text, cxn->endpoint->GetPeerId());
				}
			}

			if (g_hooking.owoify_my_outgoing_messages)
			{
				msg->setText(StringUtils::owoify(msg->text).c_str());
			}

			return OG(send_CMsgTextMessage)(mgr, con_id, msg, a4, a5);
		}
		__EXCEPTIONAL()
		{
		}
		return false;
	}

	[[nodiscard]] static bool handle_increment_stat_event(CNetGamePlayer* sender, const Hash stat_hash)
	{
		if (stat_hash == ATSTRINGHASH("awd_car_bombs_enemy_kills"))
		{
			return false;
		}

		const auto sender_ap = AbstractPlayer(sender->player_id);

		for (const auto& stat : event_stats)
		{
			if (stat_hash == stat.hash)
			{
				if (g_hooking.notify_on_commendation)
				{
					Util::toast(LANG_FMT("CMENDT_T", FMT_ARG("player", sender_ap.getName()), FMT_ARG("reason", Lang::get(stat.name))), TOAST_ALL);
				}
				return false;
			}
		}

		sender_ap.getAndApplyReactionsIn(FlowEvent::SE_INVALID, Codename("R0").toString());
		return true;
	}

	bool apply_received_script_event_reactions(AbstractPlayer sender_ap, EventAggregation&& e, int64_t* _args, int32_t _args_count, std::unordered_map<int32_t, std::string>&& index_names)
	{
		floweventreaction_t reactions = 0;
		if (sender_ap != g_player)
		{
			reactions = sender_ap.getReactions(SCRIPTED_GAME_EVENT);
		}
		reactions |= e.getReactions(sender_ap);
		if (e.types.empty())
		{
			e.types = std::vector<flowevent_t>(1, SCRIPTED_GAME_EVENT);
		}
		if (e.getCanonicalType() != FlowEvent::MOD_ID_OTHER)
		{
			if (auto tf = flow_event_reactions_to_logger_toast_flags(reactions))
			{
				std::string message = LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", e.getFullName().getLocalisedUtf8()), FMT_ARG("player", sender_ap.getName())).append(": ");
				if (e.name != ATSTRINGHASH("SE"))
				{
					message.append(LANG_GET("SE")).append(": ");
				}
				message.push_back('{');
				for (int32_t i = 0; i != _args_count; ++i)
				{
					message.append(fmt::to_string(i));
					const auto name_i = index_names.find(i);
					if (name_i != index_names.end())
					{
						message.append(" (").append(name_i->second).push_back(')');
					}
					message.append(": ");
					if (_args[i] > 0xFFFFFFFF)
					{
						message.append(fmt::to_string(_args[i]));
						message.append(" (");
						message.append(fmt::to_string((int32_t)_args[i]));
						message.push_back(')');
					}
					else
					{
						message.append(fmt::to_string((int32_t)_args[i]));
					}
					message.append(", ");
				}
				message = message.substr(0, message.length() - 2);
				message.push_back('}');
				Util::toast(std::move(message), tf);
			}
			if (auto tf = flow_event_reactions_to_toast_flags(reactions))
			{
				Util::toast(LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", e.getFullName().getLocalisedUtf8()), FMT_ARG("player", sender_ap.getName())), tf);
			}
		}
		sender_ap.applyReactions(reactions, std::move(e));
		return reactions & REACTION_BLOCK;
	}

	static bool processMiscMsg(AbstractPlayer sender, const std::string& msg)
	{
		if (auto ad_level = sender.getMessageAdLevel(msg); ad_level != 0)
		{
			if (auto gi = packet_src.getGamerInfoNoFallback())
			{
				Blacklist::addAdvertiser(*gi, ad_level);
			}

			if (ad_level > 1)
			{
				return true;
			}
		}
		return false;
	}

	static bool processSms(AbstractPlayer sender, const std::string& msg)
	{
		EventTally t(sender, FlowEvent::SMS_ANY);
		if (g_hooking.isCharFilterBypass(msg)
			|| g_hooking.isProfanityFilterBypass(msg)
			)
		{
			t.add(FlowEvent::SMS_PROFANITYBYPASS);
		}
		if (processMiscMsg(sender, msg))
		{
			t.add(FlowEvent::SMS_AD);
		}

		if (auto tf = flow_event_reactions_to_logger_toast_flags(t.reactions))
		{
			std::string str = LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", t.label.getName()), FMT_ARG("player", sender.getName()));
			if (t.label.hasNonRawName())
			{
				str.append(": ").append(t.label.raw_name.getLocalisedUtf8());
			}
			str.append(": ").append(msg);
			Util::toast(std::move(str), tf);
		}
		t.applyReactionsIn();
		return (t.reactions & REACTION_BLOCK);
	}

	bool handle_scripted_game_event(CNetGamePlayer* sender, int64_t* _args, int32_t _args_count)
	{
		AbstractPlayer sender_ap(sender->player_id);
		if (!sender_ap.isValid())
		{
			return true;
		}

		if (ScriptEventTaxonomy::isDirectPacket(_args, _args_count))
		{
			CommandPlayer* pc = sender_ap.getCommand();
			if (pc != nullptr)
			{
				pc->dp_flags = (uint8_t)ARGSIDX2(2);
				//pc->dp_flags = (uint8_t)ARGSIDX2(2) | (pc->dp_flags & (DPFLAG_COLOADING | DPFLAG_SUION)); // Copy dp_flags from this packet, but keep DPFLAG_COLOADING and DPFLAG_SUION set if they were already set.
				pc->directPacketReceived(
					DirectPacket(ARGSIDX2(3)),
					_args_count > ARGSCOUNT2(4) ? ARGSIDX2(4) : 0,
					_args_count > ARGSCOUNT2(5) ? ARGSIDX2(5) : 0,
					_args_count > ARGSCOUNT2(6) ? ARGSIDX2(6) : 0,
					_args_count > ARGSCOUNT2(7) ? ARGSIDX2(7) : 0,
					_args_count > ARGSCOUNT2(8) ? ARGSIDX2(8) : 0,
					_args_count > ARGSCOUNT2(9) ? ARGSIDX2(9) : 0
				);
			}
			return true;
		}
		else if ((hash_t)ARGSIDX2(0) == SE_CEO_JOINRES)
		{
			if (OrgHelper::hasSentJoinRequest()
				&& OrgHelper::getBoss() == sender_ap
				)
			{
				const bool accepted = (BOOL)ARGSIDX2(2);
				OrgHelper::processJoinResponse(accepted);
				if (!accepted)
				{
					return true;
				}
			}
		}

		std::unordered_map<int32_t, std::string> index_names{};
		bool ret = apply_received_script_event_reactions(sender_ap, ScriptEventTaxonomy::dissect(_args, _args_count, index_names, sender_ap), _args, _args_count, std::move(index_names));

		if (_args_count > ARGSCOUNT2(2))
		{
			/*if ((hash_t)ARGSIDX2(0) == SE_UNUSED_SMS)
			{
				auto msg = (const char*)&ARGSIDX2(2);
				auto msg_cap = (_args_count - ARGSCOUNT2(2)) * 8;
				if (msg[msg_cap - 1] == 0)
				{
					ret |= processSms(sender_ap, msg);
				}
			}
			else*/ if ((hash_t)ARGSIDX2(0) == SE_JOINED_JOB_NOTIFY)
			{
				auto msg = (const char*)&ARGSIDX2(2);
				auto msg_cap = (_args_count - ARGSCOUNT2(2)) * 8;
				if (msg[msg_cap - 1] == 0)
				{
					ret |= processMiscMsg(sender_ap, msg);
				}
			}
			/*else if ((hash_t)ARGSIDX2(0) == SE_GEN_INVITE) // SCRIPT_EVENT_FREEMODE_INVITE_GENERAL
			{
				if (_args_count > ARGSCOUNT3(12) && ARGSIDX3(12) == 0)
				{
					auto contentId = (const char*)&ARGSIDX3(9); // e.g. SV-dRRaxpUqR61NYuvL7ew (https://socialclub.rockstargames.com/job/gtav/SV-dRRaxpUqR61NYuvL7ew)
				}
			}*/
		}

		return ret;
	}

	[[nodiscard]] static std::string bitbufferToBitstring(rage::datBitBuffer& buf)
	{
		std::string str{};
		auto bits = buf.getBitLength();
		do
		{
			str.push_back(buf.readU8(1) ? '1' : '0');
		} while (--bits > 1);
		return str;
	}

	/***** NE_CRASH Codenames
	* B - Buffer Overrun
	* C - Clone Create
	* D - Clone Delete
	* E - Generic Error Catching
	* N - netGameEvent related
	* S - Scripted Game Event related
	* T - netSyncTree related
	* U - Clone Sync
	*/

	static void allow_net_event_impl(rage::netEventMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, netGameEventId_t event_id, int event_index, int event_handled_bitset, __int64 bitbuffer_size, rage::datBitBuffer* buffer)
	{
		AbstractPlayer sender_ap(sender->player_id);
		try
		{
			OG(rage_netEventMgr_ProcessEventData)(mgr, sender, receiver, event_id, event_index, event_handled_bitset, bitbuffer_size, buffer);
		}
		catch (int&)
		{
			buffer->seekEnd();
			sender_ap.getAndApplyReactionsIn(FlowEvent::SE_CRASH, "NR");
		}
		catch (...)
		{
			buffer->seekEnd();
			if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_n1_when_coloading)
			{
				sender_ap.getAndApplyReactionsIn(FlowEvent::SE_CRASH, "N1");
			}
		}
	}

	void __fastcall rage_netEventMgr_ProcessEventData(rage::netEventMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, netGameEventId_t event_id, int event_index, int event_handled_bitset, __int64 bitbuffer_size, rage::datBitBuffer* buffer)
	{
		AbstractPlayer sender_ap(sender->player_id);
		floweventreaction_t reactions = 0;
		__try
		{
			if (sender_ap == g_player)
			{
				goto allow_net_event;
			}
			if (event_id >= NET_EVENTS_END)
			{
				sender_ap.getAndApplyReactionsIn(FlowEvent::SE_CRASH, "N9");
				goto block_net_event;
			}
			Label event_name = get_net_event_name(event_id);
			EventAggregation tally({ FlowEvent::NE_ANY, event_id }, event_name);
			switch (event_id)
			{
			case NETWORK_INCREMENT_STAT_EVENT:
			{
				Hash stat_hash = buffer->readU32();
				buffer->seekStart();
				if (handle_increment_stat_event(sender, stat_hash))
				{
					goto block_net_event;
				}
				goto allow_net_event;
			}

			case SCRIPTED_GAME_EVENT:
			{
				uint32_t args_bytes = buffer->readU32();
				if (args_bytes > 0x1AF)
				{
					buffer->seekStart();
					goto block_net_event;
				}
				const int32_t args_count = args_bytes / 8;
				if (args_count > 0)
				{
					auto args = std::make_unique<int64_t[]>(args_count);
					buffer->ReadBits(args.get(), args_bytes * 8);
					if (handle_scripted_game_event(sender, args.get(), args_count))
					{
						buffer->seekStart();
						goto block_net_event;
					}
				}
				buffer->seekStart();
				goto allow_net_event;
			}

			case REMOVE_WEAPON_EVENT:
				{
					const auto data = CRemoveWeaponEvent(buffer);
					buffer->seekStart();

					if (is_session_freeroam()
						&& g_player_ped.isNetObjId(data.m_pedID)
						)
					{
						tally.add(FlowEvent::NE_DISARM);
					}

					if (!Weapon::isValidHash(data.m_weaponHash)
						// Gadgets
						&& data.m_weaponHash != ATSTRINGHASH("GADGET_PARACHUTE")
						&& data.m_weaponHash != ATSTRINGHASH("GADGET_NIGHTVISION")
						)
					{
						tally.add(FlowEvent::SE_INVALID, "N1");
					}
				}
				break;

			case REMOVE_ALL_WEAPONS_EVENT:
				{
					const auto m_pedID = buffer->readU16(13);

					if (is_session_freeroam()
						&& g_player_ped.isNetObjId(m_pedID)
						)
					{
						tally.add(FlowEvent::NE_DISARM);
					}
				}
				break;

			case GIVE_CONTROL_EVENT:
				sync_src = sender_ap;
				current_sync_type = rage::SyncType::CONTROL;
				break;

			case KICK_VOTES_EVENT:
				if (is_session_started_and_transition_finished()) // prevent rare "... is voting to kick [user's name]" while joining a session
				{
					playerbitset_t bitset = buffer->readU32(MAX_PLAYERS); static_assert(MAX_PLAYERS <= 32);
					buffer->seekStart();
					for (compactplayer_t i = 0; i != MAX_PLAYERS; ++i)
					{
						if (bitset & (1 << i))
						{
							const auto target_ap = AbstractPlayer(i);
							floweventreaction_t vk_reactions = 0;
							sender_ap.addReactions(vk_reactions, FlowEvent::MISC_VOTEKICK_ANY);
							if (target_ap == g_player)
							{
								sender_ap.addReactions(vk_reactions, FlowEvent::MISC_VOTEKICK_ME);
							}
							else
							{
								SOUP_IF_LIKELY (auto cmd = target_ap.getCommand())
								{
									cmd->vote_kick_history.emplace(sender_ap.getRockstarId());
								}
							}
							if (auto toast_flags = flow_event_reactions_to_toast_flags(vk_reactions))
							{
								Util::toast(LANG_FMT("VKR_T", FMT_ARG("player", sender_ap.getName()), FMT_ARG("target", target_ap.getName())), toast_flags);
							}
							sender_ap.applyReactions(vk_reactions);
						}
					}
				}
#ifdef STAND_DEBUG
				if (buffer->getBitLength() != MAX_PLAYERS + 1)
				{
					tally.add(FlowEvent::SE_INVALID, "N7");
				}
#endif
				break;

			case REQUEST_CONTROL_EVENT:
				{
					auto object_id = buffer->readU16(13);
					buffer->seekStart();
					if (g_player_veh.isNetObjId(object_id) && g_player_veh.isOwnerOfVehicleAndDriver())
					{
						tally.add(FlowEvent::NE_VEHTAKEOVER);
					}
				}
				break;

			case MARK_AS_NO_LONGER_NEEDED_EVENT:
				{
					uint32_t m_numScriptObjects = buffer->readU32(4);
					bool m_objectsForDeletion = buffer->readBool();
					for (uint32_t i = 0; i != m_numScriptObjects; ++i)
					{
						auto object_id = buffer->readU16(13);

						if (g_player_veh.isNetObjId(object_id) && g_player_veh.isUserPersonalVehicle())
						{
							tally.add(FlowEvent::SE_PVKICK);
						}

						if (m_objectsForDeletion)
						{
							SOUP_UNUSED(buffer->readBool());
						}
					}
					buffer->seekStart();
				}
				break;

			case SCRIPT_WORLD_STATE_EVENT:
				{
					auto type = buffer->readU8(4);
					auto change_state = buffer->readBool();

					CGameScriptId id;
					id.read(*buffer);

					if (type == NET_WORLD_STATE_ROPE)
					{
						buffer->SkipBits(9); // rope id
						buffer->SkipBits(19); // pos x (float!)
						buffer->SkipBits(19); // pos y (float!)
						buffer->SkipBits(19); // pos z (float!)
						buffer->SkipBits(19); // rot x (float!)
						buffer->SkipBits(19); // rot y (float!)
						buffer->SkipBits(19); // rot z (float!)
						float max_length = buffer->readSignedFloat(16, 100.0f);
						auto type = buffer->readI32(4);
						float init_length = buffer->readSignedFloat(16, 100.0f);
						float min_length = buffer->readSignedFloat(16, 100.0f);

						// Legit Events:
						// {Type: 1, Init Length: 0, Min Length: 0.997955, Max Length: 0.997955}

						// Crash Events:
						// {Type: 1, Init Length: 98.999, Min Length: 0.997955, Max Length: -100}
						// {Type: 4, Init Length: 98.999, Min Length: -100, Max Length: -100}

						if (type < 0
							|| max_length < 0
							|| max_length < min_length
							)
						{
							tally.add(FlowEvent::SE_CRASH, "N6");
						}
					}
					else if (type == NET_WORLD_STATE_POP_GROUP_OVERRIDE)
					{
						auto unk = buffer->readI32(8);
						auto unk2 = buffer->readU32(32);
						auto unk3 = buffer->readU32(7);
						if (unk2 == 0 && (unk3 == 0 || unk3 == 103))
						{
							tally.add(FlowEvent::SE_CRASH, "N7");
						}
					}
					else if (type == NET_WORLD_STATE_PTFX)
					{
						// If the user wants to block all PTFX, then this should also be blocked.
						if (auto reactions = sender_ap.getReactions(NETWORK_PTFX_EVENT); reactions & REACTION_BLOCK)
						{
							goto block_net_event;
						}

						auto data = CNetworkPtFXWorldStateData(buffer);

						if (is_session_freeroam()
							&& (g_player_ped.isNetObjId(data.m_EntityID)
								|| g_player_veh.isNetObjId(data.m_EntityID)
								|| data.m_FxPos.distance(g_player_ped.getPos()) < 3.0f
								)
							)
						{
#ifdef STAND_DEV
							Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Receiving a world state PTFX that's likely attached to us.").str())));
							Util::toast(fmt::format(fmt::runtime(soup::ObfusString("SCRIPT_WORLD_STATE_EVENT: {}").str()), data.toString()), TOAST_FILE);
#endif
							if (auto reactions = sender_ap.getReactions(FlowEvent::MISC_PTFX); reactions & REACTION_BLOCK) // Yes, sender is unreliable. This is better than giving the user no control.
							{
								goto block_net_event; // The event does not serialise anything about the original sender.
							}
						}
					}

					buffer->seekStart();
				}
				break;

			case SCRIPT_ENTITY_STATE_CHANGE_EVENT:
				{
					auto object_id = buffer->readU16(13);
					auto subevent_id = buffer->readU8(4);
					if (subevent_id > 9)
					{
						tally.add(FlowEvent::SE_CRASH, "N3");
					}
					//g_logger.log(fmt::format("SCRIPT_ENTITY_STATE_CHANGE_EVENT: subevent {}", subevent_id));
					if (subevent_id == 6) // SET_TASK_VEHICLE_TEMP_ACTION (CSettingOfTaskVehicleTempAction)
					{
						// This can be used to freeze/take control away from us.
						if (g_player_ped.isNetObjId(object_id))
						{
							tally.add(FlowEvent::SE_FREEZE, "N0");
						}

						auto time = buffer->readU32();
						auto object_id_2 = buffer->readU16(13);
						auto taskId = buffer->readU8();
						/*uint32_t unk = 0;
						if (buffer->readBool())
						{
							unk = buffer->readU16();
						}*/

						// In this case, `object_id` refers to the ped and `object_id_2` refers to the vehicle.

						if (auto obj = (*pointers::network_object_mgr)->find_object_by_id(object_id_2, true))
						{
							if (auto ent = obj->GetEntity())
							{
								if (g_player_veh.isValid() && g_player_veh.getPointer() == ent)
								{
									tally.add(FlowEvent::NE_VEHTAKEOVER);
								}
							}
						}

						switch (taskId)
						{
						case 15:
						case 16:
						case 17:
						case 18:
							tally.add(FlowEvent::SE_CRASH, "N4");
							break;
						}
					}
					else if (subevent_id == 9) // SET_EXCLUSIVE_DRIVER
					{
						if (g_player_veh.isNetObjId(object_id))
						{
							tally.add(FlowEvent::SE_PVKICK);
						}
					}
					buffer->seekStart();
				}
				break;

#if false
			case REPORT_MYSELF_EVENT:
				{
					auto type = buffer->readU32();
					//auto extra = buffer->readU32();
					buffer->seekStart();
					if (type < 5 || type > 10)
					{
						sender_ap.markAsModderRac(Codename("U1").toString());
					}
				}
				break;
#endif

			case NETWORK_PLAY_SOUND_EVENT:
				{
					CNetworkPlaySoundEvent data = CNetworkPlaySoundEvent(buffer);
					buffer->seekStart();

					if (data.m_soundNameHash == ATSTRINGHASH("Remote_Ring"))
					{
						tally.add(FlowEvent::NE_INFRING);
					}

					if (auto sender_cmd = sender_ap.getCommand())
					{
						if (sender_cmd->sound_timeout != 0 && GET_MILLIS_SINCE(sender_cmd->sound_timeout) < 60000)
						{
							goto block_net_event;
						}

						if (sender_cmd->sound_ratelimit.canRequest())
						{
							sender_cmd->sound_ratelimit.addRequest();
						}
						else
						{
							switch (data.m_soundNameHash)
							{
							case ATSTRINGHASH("ROUND_ENDING_STINGER_CUSTOM"): // CELEBRATION_SOUNDSET
							case ATSTRINGHASH("5s"): // MP_MISSION_COUNTDOWN_SOUNDSET
							case ATSTRINGHASH("10s"): // MP_MISSION_COUNTDOWN_SOUNDSET
							case ATSTRINGHASH("Oneshot_Final"): // MP_MISSION_COUNTDOWN_SOUNDSET
							case ATSTRINGHASH("Object_Dropped_Remote"): // GTAO_FM_Events_Soundset
							case ATSTRINGHASH("Checkpoint_Cash_Hit"): // GTAO_FM_Events_Soundset
							case ATSTRINGHASH("Event_Start_Text"): // GTAO_FM_Events_Soundset
							case ATSTRINGHASH("Return_To_Vehicle_Timer"): // GTAO_FM_Events_Soundset
							case ATSTRINGHASH("event_message_purple"):
								tally.add(FlowEvent::SE_CRASH, "N5");
								break;

							default:
								{
									if (!data.isSpammySound())
									{
										tally.add(FlowEvent::SE_INVALID, "N5");
									}
								}
							}
						}

						if (data.m_setNameHash == ATSTRINGHASH("dlc_sum20_business_battle_ac_sounds")
							&& data.m_soundNameHash == ATSTRINGHASH("air_defences_activated")
							)
						{
							if (sender_cmd->uncommon_sound_ratelimit.canRequest())
							{
								sender_cmd->uncommon_sound_ratelimit.addRequest();
							}
							else
							{
								tally.add(FlowEvent::SE_INVALID, "N5");
							}
						}

						if (is_session_freeroam())
						{
							if (!data.isSpammySound()
								&& !data.isCloseFreeroamSound()
								&& !(data.m_setNameHash == ATSTRINGHASH("gtao_biker_modes_soundset") && sender_ap.inOurOrg())
								)
							{
								if (auto sound_entity = AbstractEntity::getNetwork(data.m_EntityID); sound_entity.isValid())
								{
									if (sound_entity == g_player_ped) // Attached to us? 
									{
										tally.add(FlowEvent::SE_INVALID, "N5");
									}
									else if (sound_entity == g_player_veh && !sender_ap.getVehicle().isNetObjId(data.m_EntityID)) // Attached to our vehicle by a non-passenger?
									{
										tally.add(FlowEvent::SE_INVALID, "N5");
									}
									else if (!sender_ap.inSameInterior(g_player) && sound_entity.getPos().distanceTopdown(g_player_ped.getPos()) <= 1.5f) // Sound spawned on entity too close to us from a player not inside the same interior (or any interior)?
									{
										auto sender_ped = sender_ap.getPed();
										auto sender_veh = sender_ped.getVehicle();

										if (sound_entity != sender_ped && sound_entity != sender_veh)
										{
											// This makes us vulnerable to an attacker entering the same interior and attacking us by spawning an object near us and playing a sound on it.
											// In exchange for this edge-case, we are probably eliminating tons of false-positives that users never bother to report.
											tally.add(FlowEvent::SE_INVALID, "N5");
										}
									}
								}
								else
								{
									if (data.m_Position.distanceTopdown(g_player_ped.getPos()) <= 1.5f)
									{
										tally.add(FlowEvent::SE_INVALID, "N5");
									}
								}
							}

							const bool current_is_blocked = tally.getReactions(FlowEvent::SE_INVALID) & REACTION_BLOCK;

							if (!current_is_blocked)
							{
								g_hooking.processed_sounds.emplace_back(NetworkedSound(data.m_soundNameHash, data.m_setNameHash, data.m_SoundID));
							}

							FiberPool::queueJob([current_data{ std::move(data) }, current_is_blocked]
							{
								for (auto i = g_hooking.processed_sounds.begin(); i != g_hooking.processed_sounds.end(); )
								{
									const auto& preexisting_sound = *i;

									// Pop old sounds.
									if (preexisting_sound.m_SoundID == current_data.m_SoundID)
									{
										i = g_hooking.processed_sounds.erase(i);
										continue;
									}

									if (current_is_blocked && preexisting_sound.isSameSoundData(current_data.m_soundNameHash, current_data.m_setNameHash))
									{
										AUDIO::STOP_SOUND(preexisting_sound.m_SoundID);
										AUDIO::RELEASE_SOUND_ID(preexisting_sound.m_SoundID);
										i = g_hooking.processed_sounds.erase(i);
#ifdef STAND_DEBUG
										Util::toast(fmt::format("N5: Popped a pre-existing sound with identical information as currently-modded sound. ({}, {}, {})", current_data.m_soundNameHash, current_data.m_setNameHash, current_data.m_SoundID), TOAST_ALL);
#endif
										continue;
									}

									++i;
								}
							});
						}

						if (tally.getReactions(sender_ap) & REACTION_BLOCK)
						{
							sender_cmd->sound_timeout = get_current_time_millis();
#ifdef STAND_DEBUG
							Util::toast(fmt::format("N5: Applied sound timeout to {}.", sender_ap.getName()), TOAST_ALL);
#endif
						}
					}
				}
				break;

			case NETWORK_PTFX_EVENT:
				if (is_session_freeroam()) // Last Dose 4
				{
					const auto data = CNetworkPtFXEvent(buffer);

					buffer->seekStart();

					SOUP_IF_LIKELY (auto cmd = sender_ap.getCommand())
					{
						const auto distance = g_player_ped.getPos().distance(data.m_FxPos);
						bool modded = false;

						if (distance <= 1.0f)
						{
							if (cmd->very_close_ptfx_ratelimit.canRequest())
							{
								cmd->very_close_ptfx_ratelimit.addRequest();
							}
							else
							{
								modded = true;
							}
						}
						else if (distance <= 50.0f // Can't even see PTFX above ranges like this.
							|| data.m_bUseEntity // Attachments use relative positions.
							)
						{
							if (cmd->ptfx_ratelimit.canRequest())
							{
								cmd->ptfx_ratelimit.addRequest();
							}
							else
							{
								modded = true;
							}
						}

						if (g_player_ped.isNetObjId(data.m_EntityID) || g_player_veh.isNetObjId(data.m_EntityID))
						{
							modded = true;
						}

						if (data.m_PtFXHash == ATSTRINGHASH("scr_xt_trip_rabbit_death")) // This one is broken as fuck. Doesn't look like it was properly implemented, and just causes issues even in low quantities.
						{
							modded = true;
						}

						if (modded)
						{
							tally.add(FlowEvent::MISC_PTFX);
							cmd->ptfx_timeout = get_current_time_millis();
						}
						else
						{
							if (GET_MILLIS_SINCE(cmd->ptfx_timeout) <= 60000
								&& (sender_ap.getReactions(FlowEvent::MISC_PTFX) & REACTION_BLOCK)
								)
							{
								goto block_net_event;
							}
						}
					}
				}
				break;

			case RAGDOLL_REQUEST_EVENT:
				{
					auto target = buffer->readU16(13);

					if (g_player_ped.isNetObjId(target))
					{
						const auto reactions = sender_ap.getReactions(FlowEvent::NE_RAGDOLL);

						SOUP_IF_LIKELY (auto cmd = sender_ap.getCommand())
						{
							if (cmd->ragdoll_request_ratelimit.canRequest())
							{
								cmd->ragdoll_request_ratelimit.addRequest();
							}
							else
							{
								sender_ap.applyReactions(reactions, NuancedEventWithExtraData(FlowEvent::NE_RAGDOLL, {}, 25));
							}
						}
						
						if (reactions & REACTION_BLOCK)
						{
							goto block_net_event;
						}
					}

					buffer->seekStart();
				}
				break;

			case WEAPON_DAMAGE_EVENT:
				{
					const auto data = CWeaponDamageEvent(buffer);
					const bool is_beast = sender_ap.isBeast();

					if (data.m_weaponType == ATSTRINGHASH("weapon_tranquilizer") // This is what 0xcheat's "make him cry" sends, apparently it's supposed to kill you repeatly, even if you quit to SP: 11001100101010100010001000101111011001000010000100000000010000000000000000000000010000000000000000000000000000000000000000000000000000000000101011010100101001000100010001010100000010 (m_damageType: 3, m_weaponType: weapon_tranquilizer, m_bOverride: true, m_hitEntityWeapon: false, m_hitWeaponAmmoAttachment: false, m_silenced: true, m_damageFlags: 540688, bMeleeDamage: false, m_actionResultId: 0, m_meleeId: 0, m_meleeResultId: 0, m_weaponDamage: 0, m_bIsAggregated: false, m_weaponDamageAggregationCount: 0, m_bVictimPlayer: true, m_hitPosition_x: 0, m_hitPosition_y: 0, m_hitPosition_z: 0, m_damageTime: 2839185, m_willKillPlayer: false, m_hitObjectId: 0, m_playerDistance: 554, m_parentID: 0, m_tyreIndex: 0, m_suspensionIndex: 0, m_component: 0, m_firstBullet: true, hasVehicleData: false, useLargeDistance: false)
						|| data.m_weaponType == ATSTRINGHASH("weapon_fire") // This can be used to ragdoll us, don't think it's used legit.
						|| data.m_weaponType == ATSTRINGHASH("weapon_molotov") // Can be used to ragdoll us.
						)
					{
						tally.add(FlowEvent::SE_INVALID, "NB");
					}
					else if (data.m_weaponType == ATSTRINGHASH("WEAPON_STUNROD")) // Can be used to stunlock us
					{
						if (auto cmd = sender_ap.getCommand())
						{
							if (cmd->stun_ratelimit.canRequest())
							{
								cmd->stun_ratelimit.addRequest();
							}
							else
							{
								cmd->stun_timeout = get_current_time_millis();
								tally.add(FlowEvent::SE_INVALID, "NC");
							}

							if (GET_MILLIS_SINCE(cmd->stun_timeout) < 60'000)
							{
								goto block_net_event;
							}
						}

						if (g_gui.grace)
						{
							goto block_net_event;
						}
					}
					else
					{
						if (data.isRemoteKill(is_beast))
						{
							tally.add(FlowEvent::NE_SUDDENDEATH);
						}
						else if (data.isMagicBullet())
						{
							tally.addRaw(FlowEvent::NE_SUDDENDEATH, LANG_GET("MAGICB"));
						}
						else if (data.bMeleeDamage)
						{
							if (auto entity = AbstractEntity::getNetwork(data.m_hitObjectId); entity.isValid())
							{
								if (g_player_veh.isNetObjId(data.m_hitObjectId) || g_player_ped.isNetObjId(data.m_hitObjectId))
								{
									const auto distance = entity.getPos().distance(sender_ap.getPos());

									if (distance >= 30.0f) // If we suddenly change position after they initiate an attack, we get the event.
									{
										tally.add(FlowEvent::NE_SUDDENDEATH);
									}
								}
							}
						}

						if (!is_beast && data.isDamageExcessive())
						{
							sender_ap.triggerDetection(FlowEvent::MOD_DMGMUL, "*");
						}
					}
					
					buffer->seekStart();
					break;
				}

			case EXPLOSION_EVENT:
				{
					const auto data = CExplosionEvent(buffer);
					const auto player_pos = g_player_ped.getPos();
					const auto player_dist = data.m_explosionArgs.m_explosionPosition.distance(player_pos);

					buffer->seekStart();

					// Camera Shaking Event & Ragoll Event
					if (data.m_explosionArgs.m_bNoFx)
					{
						if (data.m_explosionArgs.m_fCamShake > 0.0f
							&& player_dist <= 30.0f
							)
						{
							tally.add(FlowEvent::NE_CAMSHAKE);
						}

						if (data.m_explosionArgs.m_sizeScale > 0.0f
							&& player_dist <= 10.0f
							)
						{
							tally.add(FlowEvent::NE_RAGDOLL);
						}
					}
					
					// Explosion Spam
					SOUP_IF_LIKELY (auto cmd = packet_src.getCommand())
					{
						if (player_dist <= 5.0f && !data.m_bHasProjectile && !data.m_shouldAttach)
						{
							if (cmd->explosion_nearby_ratelimit.canRequest())
							{
								cmd->explosion_nearby_ratelimit.addRequest();
							}
							else
							{
								auto reactions = sender_ap.getReactions(FlowEvent::NE_EXPSPAM);
								sender_ap.applyReactionsIn(LOC("EXPSPAM"), { FlowEvent::NE_EXPSPAM }, reactions, 50);
								if (reactions & REACTION_BLOCK)
								{
									goto block_net_event;
								}
							}
						}
					}

					// Modded Explosion
					if (is_session_freeroam())
					{
						bool modded = false;
						std::string extra_data{};

						// Remote Explosion
						if (!data.m_bHasProjectile
							&& !data.isProbablyDrone()
							&& data.isSenderResponsible()
							&& data.shouldHaveProjectile()
							&& data.m_explosionArgs.m_weaponHash == ATSTRINGHASH("WEAPON_EXPLOSION")
							)
						{
							modded = true;

							if (player_dist < 3.0f)
							{
								if (auto cmd = sender_ap.getCommand())
								{
									cmd->aggressive_action_warranted = true;
								}
							}
						}

						// Camera Shake Mismatch (Stand, 2Take1)
						if (data.m_explosionArgs.m_fCamShake == 0.0f
							&& data.isSenderResponsible()
							&& !data.canHaveZeroCamShake()
							)
						{
							modded = true;
							// extra_data = LANG_GET("EXPAMMO"); True for explosive ammo, but also their remote explosion.
						}

						// Explosive Ammo, Dummy Mismatch (All)
						if (data.shouldHaveDummy()
							&& !data.m_shouldAttach
							&& !data.m_hasRelatedDummy
							)
						{
							modded = true;
							extra_data = LANG_GET("EXPAMMO");
						}

						// Explosive Ammo, Absent Projectile (Kiddions)
						if (const auto weapon = Weapon::find(data.m_explosionArgs.m_weaponHash))
						{
							if (!data.m_bHasProjectile
								&& weapon->category != Weapon::MELEE
								&& weapon->hash != ATSTRINGHASH("WEAPON_RAILGUN")
								&& weapon->hash != ATSTRINGHASH("WEAPON_RAILGUNXM3")
								)
							{
								modded = true;
#ifndef STAND_DEV
								extra_data = LANG_GET("EXPAMMO");
#else
								extra_data = fmt::format(fmt::runtime(soup::ObfusString("{} / {}").str()), LANG_GET("EXPAMMO"), joaatToString(weapon->hash));
#endif
							}
						}

						// Explosive Ammo, Bullet Effect Mismatch (YimMenu "Special Ammo")
						if (auto ped = sender_ap.getPed(); ped.isValid())
						{
							if (const auto weapon = Weapon::find(ped.getSelectedWeapon())) // Hand-held weapon?
							{
								if (data.m_explosionArgs.m_explosionTag == EXP_TAG_EXPLOSIVEAMMO
									&& weapon->hash != ATSTRINGHASH("WEAPON_HEAVYSNIPER_MK2")
									&& data.m_explosionArgs.m_weaponHash == ATSTRINGHASH("WEAPON_EXPLOSION")
									)
								{
									modded = true;
#ifndef STAND_DEV
									extra_data = LANG_GET("EXPAMMO");
#else
									extra_data = fmt::format(fmt::runtime(soup::ObfusString("{} / {}").str()), LANG_GET("EXPAMMO"), joaatToString(weapon->hash));
#endif
								}
							}
						}

						// Explosive Ammo, All
						if (!data.m_shouldAttach
							&& !data.m_bHasProjectile
							&& !data.m_explosionArgs.m_bDetonatingOtherPlayersExplosive
							)
						{
							switch (data.m_explosionArgs.m_explosionTag)
							{
							case EXP_TAG_CAR:
							case EXP_TAG_BIKE:
							case EXP_TAG_BOAT:
							case EXP_TAG_PLANE:
							case EXP_TAG_TRUCK:
							case EXP_TAG_TRAIN:
							case EXP_TAG_BLIMP:
							case EXP_TAG_BLIMP2:
								modded = true;
								extra_data = LANG_GET("EXPAMMO");
								break;
							}
						}

						if (modded)
						{
							reactions |= sender_ap.triggerDetection(FlowEvent::MOD_BADEXPLOSION, std::move(extra_data), 75);
						}
					}

					// Explosion Blaming (patched client-side as of 3095, now detects menus which haven't removed the feature)
					if (!data.m_bHasProjectile
						&& data.isSenderResponsible()
						&& data.m_explodingEntityID == 0
						)
					{
						if (auto n = (*pointers::network_object_mgr)->find_object_by_id(data.m_entExplosionOwnerID, true))
						{
							if (auto ent = n->GetEntity())
							{
								if (ent->type == ENTITY_TYPE_PED)
								{
									auto ped = AbstractEntity::get((CPhysical*)ent);
									const auto sender_netobj = sender_ap.getPed().getNetObject();

									if (sender_netobj
										&& ped.isAPlayer()
										&& data.m_entExplosionOwnerID != sender_netobj->object_id // Indicates a mismatch in explosionOwner from ADD_OWNED_EXPLOSION & sender cped netobj id.
										)
									{
#if HAS_EXPBLAME
										reactions |= sender_ap.triggerDetection(FlowEvent::MOD_EXPBLAME, LANG_FMT("VICTIM", ped.getPlayer().getName()), 75);
#else
										reactions |= sender_ap.triggerDetection(FlowEvent::MOD_BADEXPLOSION, "*", 75);
#endif
									}
								}
							}
						}
					}

					if (reactions & REACTION_BLOCK)
					{
						goto block_net_event;
					}

					// Same as the other Crash Event (TC), just a different trigger.
					if (data.m_explosionArgs.m_interiorLocation.as_interiorLocation.m_interiorIndex < -1)
					{
						tally.add(FlowEvent::SE_CRASH, "TC");
					}

					break;
				}

				case BLOW_UP_VEHICLE_EVENT:
				case REQUEST_PHONE_EXPLOSION_EVENT:
					{
						const auto m_nVehicleID = buffer->readU16(13);

						if (is_session_freeroam() // Just assuming these events have legitimacy in mission vehicles or something.
							&& !sender_ap.inOurOrg() // See above. I feel like I remember some cargo missions detonatating our user vehicle too. It must be one of these events since it's not a spammed explosion, obv.
							&& g_player_veh.isNetObjId(m_nVehicleID)
							&& g_player_veh.isOwnerOfVehicleAndDriver()
							)
						{
							tally.add(FlowEvent::NE_VEHTAKEOVER);
						}

						buffer->seekStart();
					}
					break;

				case ACTIVATE_VEHICLE_SPECIAL_ABILITY_EVENT:
					{
						const auto m_vehicleID = buffer->readU16(13);

						if (g_player_veh.isNetObjId(m_vehicleID)
							&& g_player_veh.isOwnerOfVehicleAndDriver()
							)
						{
							tally.add(FlowEvent::NE_VEHTAKEOVER);
						}

						buffer->seekStart();
					}
					break;

				case CHANGE_RADIO_STATION_EVENT:
					{
						const auto m_vehicleID = buffer->readU16(13);

						if (g_player_veh.isNetObjId(m_vehicleID)
							&& sender_ap.getVehicle().getNetObjId() != m_vehicleID
							)
						{
							tally.add(FlowEvent::NE_VEHTAKEOVER);
						}

						buffer->seekStart();
					}
					break;

				case NETWORK_SOUND_CAR_HORN_EVENT:
					{
						const auto m_bIsHornOn = buffer->readBool();
						const auto m_vehicleID = buffer->readU16(13);

						if (sender_ap.getVehicle().getNetObjId() != m_vehicleID)
						{
							if (g_player_veh.isNetObjId(m_vehicleID))
							{
								tally.add(FlowEvent::NE_VEHTAKEOVER);
							}
							/*else
							{
								tally.add(FlowEvent::SE_INVALID, "NA");
							}*/
						}

						buffer->seekStart();
					}
					break;

				case GIVE_PICKUP_REWARDS_EVENT:
					if (is_session_freeroam())
					{
						const auto m_NumRewards = buffer->readU8(3);

						for (uint8_t i = 0; i < m_NumRewards; ++i)
						{
							const auto hash = buffer->readU32();

							if ((g_player_veh.isOwnerOfVehicleAndDriver() && hash == ATSTRINGHASH("REWARD_VEHICLE_FIX")) // Fixing our vehicle?
								|| (!sender_ap.inOurOrg() && (hash == ATSTRINGHASH("REWARD_HEALTH") || hash == ATSTRINGHASH("REWARD_ARMOUR"))) // Healing us?
								)
							{
								tally.add(FlowEvent::SE_INVALID, "N8");
								break;
							}
						}

						buffer->seekStart();
					}
					break;

				case DOOR_BREAK_EVENT:
					// This event is never sent, so this must be a modder.
					tally.add(FlowEvent::SE_INVALID, "N9");
					break;

				case NETWORK_SPECIAL_FIRE_EQUIPPED_WEAPON:
					{
						const auto m_entityID = buffer->readU16(13);

						if (auto n = (*pointers::network_object_mgr)->find_object_by_id(m_entityID, false))
						{
							if (auto ent = n->GetEntity())
							{
								if (ent->type == ENTITY_TYPE_PED)
								{
									if (auto weap = static_cast<CPed*>(ent)->weapon_manager->GetEquippedWeapon())
									{
										if (!weap->m_pDrawableEntity
											|| !weap->m_pDrawableEntity->archetype
											|| weap->m_pDrawableEntity->archetype->GetModelType() != MI_TYPE_WEAPON
											)
										{
											// Game would deref nullptr here. Not exactly bad because we can catch exceptions, but some co-loads can't.
											tally.add(FlowEvent::SE_CRASH, "NC");
										}
									}
								}
							}
						}

						buffer->seekStart();
					}
					break;

				case NETWORK_START_SYNCED_SCENE_EVENT: {
					const unsigned SIZEOF_POSITION = 26;
					const unsigned SIZEOF_QUAT = 30;
					const unsigned SIZEOF_ATTACH_BONE = 8;
					const unsigned SIZEOF_ANIM_HASH = 32;
					const unsigned SIZEOF_BLEND_IN = 30;
					const unsigned SIZEOF_BLEND_OUT = 30;
					const unsigned SIZEOF_ANIM_FLAGS = 32;
					const unsigned SIZEOF_RAGDOLL_BLOCKING_FLAGS = 32;
					const unsigned SIZEOF_IK_FLAGS = /*eIkControlFlags_NUM_ENUMS + 1*/ 15;
					const unsigned SIZEOF_PHASE = 9;
					const unsigned SIZEOF_RATE = 8;
					//const unsigned SIZEOF_NAME_HASH = 32;
					//const float MAX_BLEND_IN_RATE = 1001.0f;
					//const float MAX_BLEND_OUT_RATE = 1001.0f;
					const unsigned MAX_PEDS_IN_SCENE = 10;
					static const unsigned MAX_NON_PED_ENTITIES_IN_SCENE = 5;
					static const unsigned MAX_MAP_ENTITIES_IN_SCENE = 1;

					buffer->SkipBits(13); // m_SceneID
					buffer->SkipBits(32); // m_NetworkTimeStarted
					buffer->SkipBits(1); // sceneActive
					buffer->SkipBits(SIZEOF_POSITION * 3); // m_ScenePosition
					buffer->SkipBits(SIZEOF_QUAT * 4); // m_SceneRotation
					if (buffer->readBool()) // hasAttachEntity
					{
						buffer->SkipBits(13); // m_AttachToEntityID
						buffer->SkipBits(SIZEOF_ATTACH_BONE); // m_AttachToBone
					}
					if (!buffer->readBool()) // hasDefaultStopPhase
					{
						buffer->SkipBits(SIZEOF_PHASE); // m_PhaseToStopScene
					}
					if (!buffer->readBool()) // hasDefaultRate
					{
						buffer->SkipBits(SIZEOF_RATE); // m_Rate
					}
					buffer->SkipBits(1); // m_HoldLastFrame
					buffer->SkipBits(1); // m_Looped
					buffer->SkipBits(SIZEOF_PHASE); // m_Phase
					if (buffer->readBool()) // m_UseCamera
					{
						buffer->SkipBits(SIZEOF_ANIM_HASH); // m_CameraAnim
					}
					buffer->SkipBits(SIZEOF_ANIM_HASH); // m_AnimDict
					for (unsigned i = 0; i != MAX_PEDS_IN_SCENE; ++i)
					{
						if (buffer->readBool())
						{
							buffer->SkipBits(13); // m_PedID
							buffer->SkipBits(SIZEOF_ANIM_HASH); // m_AnimPartialHash
							buffer->SkipBits(SIZEOF_BLEND_IN); // m_BlendIn
							buffer->SkipBits(SIZEOF_BLEND_OUT); // m_BlendOut
							buffer->SkipBits(SIZEOF_BLEND_IN); // m_MoverBlendIn
							buffer->SkipBits(SIZEOF_ANIM_FLAGS); // m_Flags
							buffer->SkipBits(SIZEOF_RAGDOLL_BLOCKING_FLAGS); // m_RagdollBlockingFlags
							buffer->SkipBits(SIZEOF_IK_FLAGS); // m_IkFlags
						}
					}
					// Finally, the bit we're interested in...
					for (unsigned i = 0; i != MAX_NON_PED_ENTITIES_IN_SCENE; ++i)
					{
						if (buffer->readBool())
						{
							auto objId = buffer->readU16(13);
							buffer->SkipBits(SIZEOF_ANIM_HASH); // m_Anim
							buffer->SkipBits(SIZEOF_BLEND_IN); // m_BlendIn
							buffer->SkipBits(SIZEOF_BLEND_OUT); // m_BlendOut
							buffer->SkipBits(SIZEOF_ANIM_FLAGS); // m_Flags

							if (g_player_veh.isNetObjId(objId))
							{
								tally.add(FlowEvent::NE_VEHTAKEOVER);
								break;
							}
						}
					}

					buffer->seekStart();
					break;
				}

#if false
				case NETWORK_TRAIN_REPORT_EVENT:
				case NETWORK_TRAIN_REQUEST_EVENT:
					// gTrainTracks happens to overflow into CPropellerCollisionProcessor.
					// These events are not the same, but both use the first 4 bits for track index.
					auto traintrack = buffer->readU8(4);
					if (traintrack >= 12)
					{
						tally.add(FlowEvent::SE_CRASH, "NA");
					}
					buffer->seekStart();
					break;
#endif
			}
			// React
			reactions = tally.getReactions(sender_ap);
			if (auto tf = flow_event_reactions_to_logger_toast_flags(reactions))
			{
				std::stringstream stream;
				stream << LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", tally.getFullName().getLocalisedUtf8()), FMT_ARG("player", sender_ap.getName())) << ": ";
				if (tally.name != event_name)
				{
					stream << event_name.getLocalisedUtf8() << ": ";
				}
				// assuming cursor is at start
				stream << bitbufferToBitstring(*buffer);
				// now cursor is at end
				switch (event_id)
				{
				case GIVE_WEAPON_EVENT:
					{
						buffer->seekStart();
						stream << CGiveWeaponEvent(buffer).toString();
					}
					break;

				case REMOVE_WEAPON_EVENT:
					{
						buffer->seekStart();
						stream << CRemoveWeaponEvent(buffer).toString();
					}
					break;

				case REMOVE_ALL_WEAPONS_EVENT:
					{
						buffer->seekStart();
						stream << fmt::format(" ({})", g_player_ped.isNetObjId(buffer->readU16(13)));
					}
					break;

				case GIVE_PICKUP_REWARDS_EVENT:
				{
					buffer->SetCursorPos(3);
					auto reward_hash = buffer->readU32();
					stream << " {Reward: ";
					stream << joaatToString(reward_hash);
					stream << "}";
				}
				break;

				case NETWORK_PLAY_SOUND_EVENT:
					{
						buffer->seekStart();
						const auto data = CNetworkPlaySoundEvent(buffer);
						stream << data.toString();
					}
					break;

				case NETWORK_PTFX_EVENT:
					{
						buffer->seekStart();
						const auto data = CNetworkPtFXEvent(buffer);
						stream << data.toString();
					}
					break;

				case WEAPON_DAMAGE_EVENT:
					{
						buffer->seekStart();
						const auto data = CWeaponDamageEvent(buffer);
						stream << data.toString();
					}
					break;

				case EXPLOSION_EVENT:
				{
					buffer->seekStart();
					const auto data = CExplosionEvent(buffer);
					stream << data.toString();
				}
				break;

				case REQUEST_CONTROL_EVENT:
					{
						buffer->seekStart();
						auto object_id = buffer->readU16(13);
						stream << " {Object ID: ";
						stream << fmt::to_string(object_id);
						stream << "}";
					}
					break;

				case SCRIPT_ENTITY_STATE_CHANGE_EVENT:
					{
						buffer->seekStart();
						auto object_id = buffer->readU16(13);
						auto subevent_id = buffer->readU8(4);
						stream << " {Object ID: ";
						stream << fmt::to_string(object_id);
						stream << ", Subevent: ";
						stream << fmt::to_string(subevent_id);
						stream << "}";
					}
					break;

				case REPORT_MYSELF_EVENT:
					{
						buffer->seekStart();
						auto type = buffer->readU32();
						auto extra = buffer->readU32();
						stream << " {Type: ";
						stream << fmt::to_string(type);
						stream << ", Extra: ";
						stream << fmt::to_string(extra);
						stream << "}";
					}
					break;

				case CLEAR_AREA_EVENT:
					{
						buffer->seekStart();
						stream << CClearAreaEvent(buffer).toString();
					}
					break;

				case START_PROJECTILE_EVENT:
					{
						buffer->seekStart();
						stream << CStartProjectileEvent(buffer).toString();
					}
					break;

				case SCRIPT_WORLD_STATE_EVENT:
					{
						buffer->seekStart();

						auto type = buffer->readU8(4);
						auto change_state = buffer->readBool();

						CGameScriptId id;
						id.read(*buffer);

						if (type == NET_WORLD_STATE_ROPE)
						{
							buffer->SkipBits(9); // rope id
							buffer->SkipBits(19); // pos x (float!)
							buffer->SkipBits(19); // pos y (float!)
							buffer->SkipBits(19); // pos z (float!)
							buffer->SkipBits(19); // rot x (float!)
							buffer->SkipBits(19); // rot y (float!)
							buffer->SkipBits(19); // rot z (float!)
							float max_length = buffer->readSignedFloat(16, 100.0f);
							auto type = buffer->readI32(4);
							float init_length = buffer->readSignedFloat(16, 100.0f);
							float min_length = buffer->readSignedFloat(16, 100.0f);

							stream << ", Subtype: Rope, {Type: ";
							stream << type;
							stream << ", Init Length: ";
							stream << init_length;
							stream << ", Min Length: ";
							stream << min_length;
							stream << ", Max Length: ";
							stream << max_length;
							stream << "}";
						}
					}
					break;
				}
				Util::toast(stream.str(), tf);
				buffer->seekStart();
			}
			if (auto tf = flow_event_reactions_to_toast_flags(reactions))
			{
				Util::toast(LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", tally.getFullName().getLocalisedUtf8()), FMT_ARG("player", sender_ap.getName())), tf);
			}
			sender_ap.applyReactions(reactions, std::move(tally));
		}
		__EXCEPTIONAL()
		{
		}
		if (reactions & REACTION_BLOCK)
		{
		block_net_event:
			__try
			{
				pointers::send_net_event_ack(mgr, sender, receiver, event_index, event_handled_bitset);
			}
			__EXCEPTIONAL()
			{
			}
		}
		else
		{
		allow_net_event:
			allow_net_event_impl(mgr, sender, receiver, event_id, event_index, event_handled_bitset, bitbuffer_size, buffer);
		}
	}

	void __fastcall CEventGroupScriptNetwork_DispatchEvent(CEventGroupScriptNetwork* thisptr, CEventNetwork* event)
	{
		__try
		{
			const auto type_id = event->GetEventType();
			switch (type_id)
			{
#ifdef STAND_DEBUG
			case EVENT_NETWORK_REMOVED_FROM_SESSION_DUE_TO_STALL:
				Util::toast("Interesting: CEventNetworkRemovedFromSessionDueToStall");
				break;

			case EVENT_VOICE_SESSION_STARTED:
				Util::toast("Interesting: CEventNetworkVoiceSessionStarted", TOAST_ALL);
				break;

			case EVENT_VOICE_SESSION_ENDED:
				Util::toast("Interesting: CEventNetworkVoiceSessionEnded", TOAST_ALL);
				break;

			case EVENT_VOICE_CONNECTION_REQUESTED:
				Util::toast("Interesting: CEventNetworkVoiceConnectionRequested", TOAST_ALL);
				break;

			case EVENT_VOICE_CONNECTION_TERMINATED:
				Util::toast("Interesting: CEventNetworkVoiceConnectionTerminated", TOAST_ALL);
				break;

			case EVENT_NETWORK_FOLLOW_INVITE_RECEIVED:
				Util::toast("Interesting: CEventNetworkFollowInviteReceived", TOAST_ALL);
				break;
#endif

			case EVENT_NETWORK_REMOVED_FROM_SESSION_DUE_TO_COMPLAINTS:
				if (g_hooking.toast_love_letter_kick
					&& !is_session_transition_active(true)
					)
				{
					Util::toast(LOC("LLKT_T"), TOAST_ALL);
				}
				break;

			case EVENT_NETWORK_CONNECTION_TIMEOUT:
				{
					int args[1 * 2]{};
					event->RetrieveData(args, sizeof(args));
					if (auto gamer_info = AbstractPlayer(args[0]).getGamerInfoNoFallback())
					{
						LeaveReasons::onTimedOut(*gamer_info);
					}
				}
				break;

			case EVENT_NETWORK_PLAYER_COLLECTED_AMBIENT_PICKUP:
			{
				int args[9 * 2]{};
				event->RetrieveData(args, sizeof(args)); // Within the function we are calling here, it has a size check. If it doesn't match, we don't get the data.

				if (args[4] != g_player)
				{
					break;
				}
				std::string pickup_toast{};
				floweventreaction_t reactions = g_hooking.flow_event_reactions[FlowEvent::PUP_ANY].getReactions();
				switch (args[0])
				{
				case ATSTRINGHASH("pickup_money_variable"):
				case ATSTRINGHASH("PICKUP_VEHICLE_MONEY_VARIABLE"):
				case ATSTRINGHASH("pickup_money_case"):
				case ATSTRINGHASH("pickup_money_wallet"):
				case ATSTRINGHASH("pickup_money_purse"):
				case ATSTRINGHASH("pickup_money_dep_bag"):
				case ATSTRINGHASH("pickup_money_med_bag"):
				case ATSTRINGHASH("pickup_money_paper_bag"):
				case ATSTRINGHASH("pickup_money_security_case"):
				case ATSTRINGHASH("pickup_gang_attack_money"):
					// Cash
					pickup_toast = LANG_GET("PTX_CASHPUP_T");
					reactions |= g_hooking.flow_event_reactions[FlowEvent::PUP_CASH].getReactions();
					break;

				case ATSTRINGHASH("pickup_custom_script"):
				case ATSTRINGHASH("PICKUP_PORTABLE_CRATE_FIXED_INCAR"):
				case ATSTRINGHASH("PICKUP_PORTABLE_CRATE_UNFIXED_INCAR_SMALL"):
				case ATSTRINGHASH("PICKUP_PORTABLE_CRATE_UNFIXED_INCAR_WITH_PASSENGERS"):
					if (args[6] == ATSTRINGHASH("bkr_prop_coke_boxeddoll") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_sasquatch") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_beast") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_rsrgeneric") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_rsrcomm") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_pogo") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_prbubble") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_imporage") ||
						args[6] == ATSTRINGHASH("vw_prop_vw_colle_alien"))
					{
						if (((unsigned int)args[2]) > 99)
						{
							// Invalid
							pickup_toast = LANG_GET("PTX_IPUP_T");
							reactions |= g_hooking.flow_event_reactions[FlowEvent::PUP_INVALID].getReactions();
						}
						else
						{
							// RP
							pickup_toast = LANG_GET("PTX_RPPUP_T");
							reactions |= g_hooking.flow_event_reactions[FlowEvent::PUP_RP].getReactions();
						}
						break;
					}
					if (args[6] == ATSTRINGHASH("vw_prop_vw_lux_card_01a"))
					{
						if (((unsigned int)args[2]) > 53)
						{
							// Invalid
							pickup_toast = LANG_GET("PTX_IPUP_T");
							reactions |= g_hooking.flow_event_reactions[FlowEvent::PUP_INVALID].getReactions();
						}
						else
						{
							// RP
							pickup_toast = LANG_GET("PTX_RPPUP_T");
							reactions |= g_hooking.flow_event_reactions[FlowEvent::PUP_RP].getReactions();
						}
						break;
					}
					[[fallthrough]];
				default:
					// Any
					if (reactions & REACTION_TOAST)
					{
						Util::toast(LANG_FMT("PTX_APUP_T", joaatToString(args[6])), TOAST_DEFAULT);
					}
					break;
				}
				if (auto tf = flow_event_reactions_to_logger_toast_flags(reactions))
				{
					Util::toast(std::move(std::string(pickup_toast.empty() ? LANG_GET("PTX_APUP_L") : pickup_toast).append(fmt::format(": {}, {}, {}, {}, {}, {}, {}", args[0], args[1], args[2], args[3], args[4], args[5], joaatToString(args[6])))), tf);
				}
				if (!pickup_toast.empty())
				{
					if (auto tf = flow_event_reactions_to_toast_flags(reactions))
					{
						Util::toast(std::move(pickup_toast), tf);
					}
				}
				if (reactions & REACTION_BLOCK)
				{
					return;
				}
			}
			break;

			case EVENT_NETWORK_DAMAGE_ENTITY:
				if (!is_session_transition_active(true))
				{
					int args[13 * 2]{};
					event->RetrieveData(args, sizeof(args));

					if (Hash weapon = args[6 * 2]; weapon != ATSTRINGHASH("WEAPON_EXPLOSION"))
					{
#if INVALID_GUID == -1
						AbstractEntity attacker_ped = AbstractEntity::get(args[1 * 2]);
#else
						AbstractEntity attacker_ped = AbstractEntity::invalid();
						if (args[1 * 2] != -1)
						{
							attacker_ped = AbstractEntity::get(args[1 * 2]);
						}
#endif
						if (attacker_ped.isAPlayer())
						{
							auto attacker = attacker_ped.getPlayer();

							// Attacking While Invulnerable
							if (attacker_ped.isInvulnerable()
								&& !attacker_ped.isDead()
								&& !attacker.isInInterior() // proxmine would be valid for any interior, also missiles like in terrorbyte or kosatka
								&& !attacker.isUsingRcVehicle()
								&& !(attacker.getReactions(FlowEvent::SYNCIN_CLONE_UPDATE, false) & REACTION_BLOCK)
								)
							{
								auto* attacker_cmd = attacker.getCommand();
								if (attacker_cmd != nullptr
									&& GET_MILLIS_SINCE(attacker_cmd->invulnerable_since) > 3000
									&& GET_MILLIS_SINCE(attacker_cmd->latest_death_at) > 3000
									)
								{
									attacker_cmd->getAndApplyReactions(attacker, FlowEvent::MOD_GODATK, 75);
								}
							}

							// Modded Driveby Weapon
							{
								// Check if attacker is in a vehicle
								if (auto model = attacker.getCurrentVehicleModel(); model != 0)
								{
									// Ensure weapon is still selected
									if (auto cped = attacker_ped.getCPed();
										cped
										&& cped->weapon_manager
										&& cped->weapon_manager->selector.selected_weapon_hash == weapon
										)
									{
										if (auto* attacker_cmd = attacker.getCommand())
										{
											// Check if attacker was in vehicle for at least 5 seconds
											if (GET_MILLIS_SINCE(attacker_cmd->latest_vehicle_entry_at) >= 5000)
											{
												if (auto w = Weapon::find(weapon))
												{
													if (auto veh_model = model.getVehicleModelInfo())
													{
														// Finally, check if the weapon is not allow for vehicle
														if (veh_model->m_pVehicleLayoutInfo
															&& !veh_model->m_pVehicleLayoutInfo->doesAnySeatAllowWeapon(*w)
															)
														{
															attacker_cmd->getAndApplyReactions(attacker, FlowEvent::MOD_DRIVEB, fmt::format(fmt::runtime(soup::ObfusString("{} / {}").str()), Weapon::find(weapon)->getName(), model.getLabel().getLiteralUtf8()));
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

					/*Ped attackee_ped = args[0 * 2];
					Ped attacker_ped = args[1 * 2];
					if (attackee_ped != attacker_ped)
					{
						if (PED::IS_PED_A_PLAYER(attackee_ped) && PED::IS_PED_A_PLAYER(attacker_ped))
						{
							Player attackee = NETWORK::NETWORK_GET_PLAYER_INDEX_FROM_PED(attackee_ped);
							Player attacker = NETWORK::NETWORK_GET_PLAYER_INDEX_FROM_PED(attacker_ped);

							// damage (float): args[2 * 2]

							util::toast(fmt::format(
								"{} damaged {} with {}{}{}",
								AbstractPlayer(attackee).get_name(),
								AbstractPlayer(attacker).get_name(),
								reverseJoaat(args[6 * 2]),
								args[5 * 2] == 1 ? " fatally" : "",
								AbstractEntity::get(attacker_ped).is_invulnerable() ? " while invulnerable" : ""
							), TOAST_ALL, false);
						}
					}*/
				}
				break;

			case EVENT_NETWORK_PRIMARY_CLAN_CHANGED:
				if (g_hooking.block_bail_crew)
				{
					return;
				}
				break;

			case EVENT_TEXT_MESSAGE_RECEIVED:
				{
					constexpr auto args_size = (29 * 2);
					auto args = std::make_shared<int[]>(args_size);
					event->RetrieveData(args.get(), args_size * sizeof(int));

					AbstractPlayer sender = packet_src;
					Player blamed_sender = NETWORK::NETWORK_GET_PLAYER_FROM_GAMER_HANDLE((Any*)&args[16 * 2]);
					if (!packet_src.isValid())
					{
						sender = blamed_sender;
					}

					if (processSms(sender, (const char*)&args[0]))
					{
						return;
					}
				}
				break;
			}
			return OG(CEventGroupScriptNetwork_DispatchEvent)(thisptr, event);
		}
		__EXCEPTIONAL()
		{
		}
	}

	bool __fastcall rage_netPlayerMgrBase_SendBuffer(rage::netPlayerMgrBase* a1, const rage::netPlayer* recipient, const void* data, unsigned int size, unsigned int sendFlags, rage::netSequence* seq, const rage::netPlayer* sender)
	{
		__try
		{
			rage::datBitBuffer buf;
			buf.SetReadOnlyBytes(data, size);
			const auto id = rage::netMessage::readHeader(buf);
			//g_logger.log(fmt::format("Sending {} to {}", getPacketName(netMsgId), a2->GetLogName()));
			if (id == REQUEST_OBJECT_IDS_MSG)
			{
				//g_logger.log(fmt::format("Sending RequestObjectIdsMsg to {}", receiver->GetLogName()));
				CommandBlockBlockJoin::pending_object_ids.emplace_back(SessionPlayer(*recipient->GetGamerInfoImpl()));
			}
			else if (id == MSG_SCRIPT_JOIN)
			{
				/*CGameScriptId script_id;
				script_id.read(buf);
				Util::toast(fmt::format("Sending MSG_SCRIPT_JOIN for {}", script_id.m_hash), TOAST_ALL);*/

				if (CommandBlockBlockJoin::resend_script_join)
				{
					CommandBlockBlockJoin::resend_script_join = false;
				}
				else
				{
					CommandBlockBlockJoin::pending_script_join_acks.emplace_back(SessionPlayer(*recipient->GetGamerInfoImpl()));
				}
			}
			return OG(rage_netPlayerMgrBase_SendBuffer)(a1, recipient, data, size, sendFlags, seq, sender);
		}
		__EXCEPTIONAL()
		{
		}
		return false;
	}

	CBaseModelInfo* __fastcall rage_fwArchetypeManager_GetArchetypeFromHashKey(Hash modelHash, unsigned int* index)
	{
		SOUP_IF_LIKELY (g_gta_module.range.contains(_ReturnAddress()))
		{
			EXCEPTIONAL_LOCK(g_hooking.first_model_info_request_times_mtx)
			if (g_hooking.first_model_info_request_times.find(modelHash) == g_hooking.first_model_info_request_times.end())
			{
				g_hooking.first_model_info_request_times.emplace(modelHash, get_current_time_millis());
			}
			EXCEPTIONAL_UNLOCK(g_hooking.first_model_info_request_times_mtx)
		}
		SOUP_IF_UNLIKELY (AbstractModel(modelHash).isNull())
		{
			// Some 2take1 luas use the fact that "slod_human" fails to load as a "coloading with Stand" detection
			if (!ColoadMgr::coloading_with_2take1menu)
			{
				return nullptr;
			}
		}
		return OG(rage_fwArchetypeManager_GetArchetypeFromHashKey)(modelHash, index);
	}

	long onScriptException(_EXCEPTION_POINTERS* exp) noexcept
	{
		rage::scrThread* const thread = rage::scrThread::get();

		thread->m_context.m_instruction_pointer = script_vm_helper_get_last_ins_ptr();
		thread->m_context.m_instruction_pointer += getOpcodeArgLength(thread->m_context.getCodePtr());
		thread->m_context.m_stack_pointer = uint32_t((script_vm_helper_get_last_stack_ptr() - (uint64_t)thread->m_stack) / 8) + 1;
		thread->m_context.m_frame_pointer = uint32_t((script_vm_helper_get_last_frame_ptr() - (uint64_t)thread->m_stack) / 8);
		/*for (int i = -3; i <= 3; i++)
		{
			g_logger.log(fmt::format("{}: {}", i, *(thread->getCodePtr() + i)));
		}*/
		std::string message = LANG_FMT("SVM_EXP", FMT_ARG("script", thread->m_name));
		blameScriptCrash(thread, message, false);
		if (script_error_recovery_mode == ScriptErrorRecoveryMode::DISABLED
			|| (script_error_recovery_mode == ScriptErrorRecoveryMode::FREEMODE_ONLY && thread->m_context.m_script_hash != ATSTRINGHASH("freemode"))
			)
		{
			thread->m_context.m_state = rage::scrThread::ABORTED;
		}
		auto exception_name = Exceptional::getExceptionName(exp);
		exception_name.insert(0, "Caught ");
		Exceptional::report(std::move(exception_name), std::move(message), exp);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	static uint32_t __fastcall execute_script(void** stack, int64_t** script_globals, rage::scrProgram* program, rage::scrThreadContext* thread_ctx)
	{
		uint32_t ret = rage::scrThread::ABORTED;

		g_hooking.executing_script_name = program->m_name;

#if SVM_REIMPL
		if (g_use_script_vm_reimpl)
		{
			__try
			{
				ret = rage::script_vm_reimpl(reinterpret_cast<rage::scrValue*>(stack), reinterpret_cast<rage::scrValue**>(script_globals), *program, thread_ctx);
			}
			__except (onScriptException(GetExceptionInformation()))
			{
			}
		}
		else
#endif
		{
			script_vm_helper_reset_ins_count();
			__try
			{
				ret = g_hooking.script_vm_hook.getOriginal<decltype(&script_vm)>()(stack, script_globals, program, thread_ctx);
			}
			__except (onScriptException(GetExceptionInformation()))
			{
			}
		}
		if (ret == rage::scrThread::STAND_INFLOOP)
		{
#ifdef STAND_DEBUG
			Util::toast(fmt::format("{} script froze at {} in func_{}", program->m_name, thread_ctx->m_instruction_pointer, program->getFuncIndexByCodeIndex(thread_ctx->m_instruction_pointer) - 1), TOAST_ALL);
#else
			Util::toast(LANG_FMT("SVM_FRZ", program->m_name), TOAST_ALL);
#endif

			thread_ctx->m_state = rage::scrThread::ABORTED;
			ret = rage::scrThread::ABORTED;

			if (thread_ctx->m_script_hash == ATSTRINGHASH("freemode"))
			{
				ScriptVmErrorHandling::onHadToKillfreemode();
			}
		}

		g_hooking.executing_script_name = nullptr;

		return ret;
	}

	uint32_t __fastcall script_vm(void** stack, int64_t** script_globals, rage::scrProgram* program, rage::scrThreadContext* thread_ctx)
	{
		uint32_t ret = rage::scrThread::ABORTED;

		switch (thread_ctx->m_script_hash)
		{
		case ATSTRINGHASH("valentinerpreward2"):
			if (!g_hooking.block_background_script)
			{
				goto allow_script_execution;
			}
			/*if (*pointers::game_state == 0)
			{
				SHIT_BACKGROUND_SCRIPTS_SAY
			}*/
			ret = rage::scrThread::RUNNING;
			break;

		case ATSTRINGHASH("am_armybase"):
		case ATSTRINGHASH("restrictedareas"):
		case ATSTRINGHASH("re_armybase"):
		case ATSTRINGHASH("re_lossantosintl"):
		case ATSTRINGHASH("re_prison"):
		case ATSTRINGHASH("re_prisonvanbreak"):
		case ATSTRINGHASH("am_doors"):
			if (!g_hooking.disable_restricted_areas || is_session_transition_active(true))
			{
				goto allow_script_execution;
			}
			ret = rage::scrThread::RUNNING;
			break;

		default:
		allow_script_execution:
			int i = 0;
			do
			{
				ret = execute_script(stack, script_globals, program, thread_ctx);
			} while (ret == rage::scrThread::STAND_ERROR && ++i != 10);
			break;
		}

		return ret;
	}

	std::pair<int, int*> getLastScriptEvent(hash_t script)
	{
		const auto script_i = g_hooking.last_script_event_map.find(script);
		if (script_i != g_hooking.last_script_event_map.end())
		{
			return { script_i->second.first, script_i->second.second.get() };
		}
		return { 0, nullptr };
	}

	std::string dumpScriptEvent(std::pair<int, int*>& event)
	{
		std::string dump{};
		for (int i = 0; i < event.first; i++)
		{
			StringUtils::list_append(dump, event.second[i * 2]);
		}
		return dump;
	}

	bool blameScriptCrash(rage::scrThread* thread, std::string& message, bool include_event_dump)
	{
		auto event = getLastScriptEvent(thread->m_context.m_script_hash);
		if (event.first != 0)
		{
			AbstractPlayer sender(event.second[1 * 2]);
			if (sender.isValid())
			{
				message.push_back(' ');
				message.append(
					include_event_dump ? LANG_FMT("SVM_BLM_D", FMT_ARG("player", sender.getName()), FMT_ARG("event", dumpScriptEvent(event)))
					: LANG_FMT("SVM_BLM_S", FMT_ARG("player", sender.getName()))
				);
			}
			return true;
		}
		return false;
	}

	void logScriptScrash(rage::scrThread* thread, std::string&& message)
	{
		blameScriptCrash(thread, message, true);
		Util::toast(std::move(message), TOAST_ALL);
	}

	void __fastcall camFrame_copy(camFrame* a1, camFrame* a2)
	{
		__try
		{
			OG(camFrame_copy)(a1, a2);

			if (g_hooking.fov_override != FLT_MAX)
			{
				float fov = g_hooking.fov_override;
				bool both = (fov != 360.0f);

				if (fov == 0.0f)
				{
					fov = 360.0f;
				}

				// This heuristic works to get the current FOV by setting to 0 every tick after reading it.
				/*if (g_hooking.current_fov == 0.0f)
				{
					g_hooking.current_fov = -1.0f;
				}
				else if (g_hooking.current_fov == -1.0f)
				{
					g_hooking.current_fov = a2->fov;
				}*/

				a1->fov = fov;
				if (both)
				{
					a2->fov = fov;
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	bool CPhysical_TestNoCollision(CPhysical* _this, rage::phInst* pOtherInst)
	{
		if (pOtherInst
			&& CommandDriveOnWater::object.isValid()
			&& CommandDriveOnWater::object.getPointer() == _this
			)
		{
			return g_player_veh.getPointer() != pOtherInst->getPhysical();
		}

		return OG(CPhysical_TestNoCollision)(_this, pOtherInst);
	}

	CProjectile* CProjectileManager_CreateProjectile(uint32_t uProjectileNameHash, uint32_t uWeaponFiredFromHash, CEntity* pOwner, const void* mat, float fDamage, int damageType, int effectGroup, const CEntity* pTarget, const void* pNetIdentifier, uint32_t tintIndex, bool bCreatedFromScript, bool bProjectileCreatedFromGrenadeThrow)
	{
		if (g_hooking.patch_projectile_pool)
		{
			__try
			{
				if (!pointers::projectile_pool->canAppend())
				{
					CProjectile* oldest_projectile = nullptr;
					float highest_age = FLT_MIN;
					for (int i = 0; i != pointers::projectile_pool->m_Count; ++i)
					{
						if (pointers::projectile_pool->elements[i])
						{
							if (pointers::projectile_pool->elements[i]->age > highest_age)
							{
								oldest_projectile = pointers::projectile_pool->elements[i];
								highest_age = pointers::projectile_pool->elements[i]->age;
							}
						}
					}
					if (oldest_projectile != nullptr) // Somehow, the projectile pool could be full of nullptrs, in which case there is nothing to do.
					{
						oldest_projectile->Destroy();
					}
				}
			}
			__EXCEPTIONAL()
			{
			}
		}

		return OG(CProjectileManager_CreateProjectile)(uProjectileNameHash, uWeaponFiredFromHash, pOwner, mat, fDamage, damageType, effectGroup, pTarget, pNetIdentifier, tintIndex, bCreatedFromScript, bProjectileCreatedFromGrenadeThrow);
	}

	rage::rlGamerHandle* __fastcall rlGamerHandle_from_friend_index(int32_t friend_index)
	{
		rage::rlGamerHandle* gamerHandle = g_hooking.rlGamerHandle_from_friend_index_hook.getOriginal<decltype(&rlGamerHandle_from_friend_index)>()(friend_index);
		if (g_hooking.replace_friends_rid)
		{
			gamerHandle->rockstar_id = g_hooking.replacement_rid;
			g_hooking.replace_friends_rid = false;
		}
		return gamerHandle;
	}

	bool rage_rlGetGamerStateTask_ParseResults(rage::rlGetGamerStateTask* task)
	{
		__try
		{
			for (uint32_t i = 0; i < task->num_tasks; ++i)
			{
				const char* response = task->responses[i];
				if (response == nullptr)
				{
					continue;
				}
				//g_logger.log(response);
				rapidjson::Document document;
				if (!document.Parse(response).HasParseError() && document.IsObject())
				{
					const auto rid = str2int<uint64_t>(document["_id"].GetString() + 3).value();
					if (auto* g = RemoteGamer::getSimple(rid))
					{
						std::string gsinfo = document.FindMember(soup::ObfusString("gsinfo").c_str())->value.GetString();
						std::optional<int64_t> gstype{};
						auto mem_gstype = document.FindMember(soup::ObfusString("gstype").c_str());
						if (mem_gstype != document.MemberEnd())
						{
							gstype = mem_gstype->value.GetInt64();
						}

						g->response_from_detailed = false;
						if (gsinfo.empty())
						{
							if (!gstype.has_value())
							{
								g->processGamerState(false, "", 0); // offline
							}
							else
							{
								g->processGamerState(true, "", 0); // story mode
							}
						}
						else
						{
							SOUP_IF_LIKELY (gstype.has_value())
							{
								g->processGamerState(true, std::move(gsinfo), gstype.value());
							}
							else
							{
								// If no value, default to public. Rare, but can happen.
								g->processGamerState(true, std::move(gsinfo), GS_PUBLIC);
							}
						}
					}
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		__try
		{
			return OG(rage_rlGetGamerStateTask_ParseResults)(task);
		}
		__EXCEPTIONAL()
		{
		}
		return false;
	}

#if false
	static uint32_t og_maxResults;

	bool __fastcall rage_rlScMatchmaking_Find(int userIndex, unsigned int min_available_slots, const rage::rlMatchingFilter& filter, unsigned int maxResults, rage::rlSessionInfo* outResults, unsigned int* outNumResults, rage::netStatus* outStatus)
	{
		if (!Matchmaking::isRequestActive())
		{
#if PRINT_MMREQUESTS
			__try
			{
				std::string msg = "Matchmaking request: ";
				msg.append("filter_id = ").append(fmt::to_string(filter.filter_id)).append(", ");
				msg.append("filter_name = ").append(filter.filter_name).append(", ");
				msg.append("conditions = [");
				for (uint32_t i = 0; i != filter.num_conditions; ++i)
				{
					msg.append(filter.attribute_names[filter.conditions_lefthand_attr_ids_index[i]]);
					msg.push_back(' ');
					msg.append((const char*)&filter.conditions_operators[i]);
					msg.push_back(' ');
					msg.append(fmt::to_string(filter.conditions_righthand_values[i]));
					//msg.append((filter.conditions_righthand_values_mask >> i) ? fmt::to_string(filter.conditions_righthand_values[i]) : "[no value]");
					if (i != filter.num_conditions - 1)
					{
						msg.append(", ");
					}
				}
				msg.append("]");
				g_logger.log(std::move(msg));
			}
			__EXCEPTIONAL()
			{
			}
#endif
			if (g_hooking.matchmaking_join)
			{
				g_hooking.matchmaking_join = false;
				outResults[0] = g_hooking.matchmaking_join_session;
				*outNumResults = 1;
				outStatus->m_StatusCode = rage::netStatus::SUCCEEDED;
				return true;
			}
			if (g_hooking.disable_matchmaking)
			{
				outStatus->m_StatusCode = rage::netStatus::FAILED;
				return true;
			}
			g_hooking.magnet_matchmaking_this_request = g_hooking.magnet_matchmaking;
			if (g_hooking.magnet_matchmaking_this_request != 0)
			{
				og_maxResults = maxResults;

				maxResults = Matchmaking::MAX_RESULTS;
			}
		}
		return OG(rage_rlScMatchmaking_Find)(userIndex, min_available_slots, filter, maxResults, outResults, outNumResults, outStatus);
	}

	bool __fastcall rage_rlScMatchmakingFindTask_ReadSession(rage::rlScMatchmakingFindTask* task, rage::parTreeNode* node, rage::rlSessionInfo* outSession)
	{
		bool ret = OG(rage_rlScMatchmakingFindTask_ReadSession)(task, node, outSession);
		__try
		{
			if (Matchmaking::isRequestActive()
				|| g_hooking.magnet_matchmaking_this_request != 0
				)
			{
				const auto data_node = node->m_Root;
				const auto attributes_node = data_node->m_Next;
				const std::string raw_attributes = attributes_node->m_Data;
				const auto attributes = soup::string::explode(raw_attributes, ',');
				const auto discriminator = static_cast<uint32_t>(str2int<int32_t>(attributes[2]).value());
				uint8_t port = (discriminator & 0b11111111);
				uint8_t version = ((discriminator >> 8) & 0b1111);
				uint8_t build = ((discriminator >> 12) & 0b11);
				bool exclude = ((discriminator >> 14) & 0b1); // seems to be true for bad sport lobbies (maybe also cheater lobbies but there are none)
				bool visible = ((discriminator >> 15) & 0b1);
				bool in_progress = ((discriminator >> 16) & 0b1);
				bool free_aim = ((discriminator >> 17) & 0b1);
				uint16_t user = (discriminator >> 18);
				
				SessionAttributes attrs;
				attrs.user = user;
				attrs.visible = visible;
				attrs.bad_sport = exclude;
				attrs.free_aim = free_aim;
				attrs.players = static_cast<compactplayer_t>(str2int<int32_t>(attributes[4]).value());
				attrs.region = static_cast<uint8_t>(str2int<int32_t>(attributes[6]).value());

#if false
				g_logger.log(fmt::format("Data: {}", data_node->m_Data));
				//g_logger.log(fmt::format("- Session: ID {:x}, hosted by {}", outSession->id, outSession->host.gamer_handle.rockstar_id));
				g_logger.log(fmt::format("- Attributes: {}", raw_attributes));
				g_logger.log(fmt::format("- Discriminator: port={}, version={}, build={}, exclude={}, visible={}, in_progress={}, free_aim={}, user={}",
					port, version, build, exclude, visible, in_progress, free_aim, user
				));
				g_logger.log(fmt::format("- Players: {}", attrs.players));
				// Attributes: A,B,CCCCCCCCCC,D,E,FF,G,H,I,J
				// Attributes: 0,0,-542374558,0,1,10,7,0,0,0
				// C / [2] = Discriminator
				// E / [4] = Num players (can be 0)
				// F / [5] = MMATTR_AIM_TYPE
				// G / [6] = MMATTR_REGION

				static_assert(STAND_DEBUG);
#endif

				Matchmaking::session_attributes.emplace(outSession->getIdHash(), std::move(attrs));
			}
		}
		__EXCEPTIONAL()
		{
		}
		return ret;
	}

	bool __fastcall rage_rlScMatchmakingFindTask_ProcessSuccess(rage::rlScMatchmakingFindTask* task, rage::rlRosResult* a2, rage::parTreeNode* node, int* a4)
	{
		__try
		{
			const bool modify_results = (g_hooking.magnet_matchmaking_this_request != 0 && !Matchmaking::isRequestActive());
			auto og_results = task->results;
			if (modify_results)
			{
				task->results = Matchmaking::results;
			}
			if (OG(rage_rlScMatchmakingFindTask_ProcessSuccess)(task, a2, node, a4))
			{
				if (modify_results)
				{
					task->results = og_results;
					std::sort(Matchmaking::results, Matchmaking::results + task->num_results, [](const rage::rlSessionInfo& a, const rage::rlSessionInfo& b)
					{
						if (a.id == 0)
						{
							return false;
						}
						if (b.id == 0)
						{
							return true;
						}

						const SessionAttributes& a_attrs = Matchmaking::getSessionAttributes(a.getIdHash());
						const SessionAttributes& b_attrs = Matchmaking::getSessionAttributes(b.getIdHash());

						auto a_dist = abs(a_attrs.players - g_hooking.magnet_matchmaking);
						auto b_dist = abs(b_attrs.players - g_hooking.magnet_matchmaking);

						return a_dist < b_dist;
					});
					Matchmaking::clear();
					if (task->num_results < og_maxResults)
					{
						og_maxResults = task->num_results;
					}
					if constexpr (false)
					{
						for (uint32_t i = 0; i < og_maxResults; ++i)
						{
							g_logger.log(fmt::format("Candidate session {} with {} players", i, Matchmaking::getSessionAttributes(Matchmaking::results[i].getIdHash()).players));
							og_results[i] = Matchmaking::results[i];
						}
					}
					else
					{
						memcpy(og_results, Matchmaking::results, og_maxResults * sizeof(rage::rlSessionInfo));
					}
					if (task->num_results_out)
					{
						*task->num_results_out = og_maxResults;
					}
				}
				return true;
			}
		}
		__EXCEPTIONAL()
		{
		}
		return false;
	}
#endif

	// Without matchmaking bias (rage_netTransactor_SendResponse_rage_rlSessionDetailResponse), this is useless.
	bool rage_rlScMatchmaking_Advertise(int localGamerIndex, uint32_t numSlots, uint32_t availableSlots, const rage::rlMatchingAttributes* attrs, uint64_t sessionId, const rage::rlSessionInfo* sessionInfo, rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status)
	{
		if (g_hooking.player_magnet)
		{
			status->m_Status = rage::netStatus::PENDING;

			FiberPool::queueJob([=]
			{
				// Publishing a legitimate advertisement allows us to use rage_rlScMatchmaking_Unadvertise to attach multiplexed advertisement cleanup to legitimate advertisement cleanup.
				rage::netStatus legitimate;
				if (!OG(rage_rlScMatchmaking_Advertise)(localGamerIndex, numSlots, availableSlots, attrs, sessionId, sessionInfo, matchId, &legitimate))
				{
					status->m_Status = rage::netStatus::FAILED;
					return;
				}

				legitimate.waitUntilDone();
				if (legitimate.m_StatusCode != rage::netStatus::SUCCEEDED)
				{
					status->m_Status = rage::netStatus::FAILED;
					return;
				}

				for (uint8_t i = 0; i != 10; ++i)
				{
					rage::netStatus multiplex_status{};
					rage::rlScMatchmakingMatchId id{};
					if (OG(rage_rlScMatchmaking_Advertise)(localGamerIndex, numSlots, availableSlots, attrs, sessionId, sessionInfo, &id, &multiplex_status))
					{
						multiplex_status.waitUntilDone();
						if (multiplex_status.m_StatusCode == rage::netStatus::SUCCEEDED)
						{
							g_hooking.session_advertisements.emplace_back(std::move(id));
						}
					}
				}

				status->m_StatusCode = rage::netStatus::SUCCEEDED;
			});

			return true;
		}

		return OG(rage_rlScMatchmaking_Advertise)(localGamerIndex, numSlots, availableSlots, attrs, sessionId, sessionInfo, matchId, status);
	}

	// The game tries to unadvertise our multiplexed advertisements shortly after we publish them.
	// The game only tries to unadvertise legitimate advertisements when we disconnect from the session.
	bool rage_rlScMatchmaking_Unadvertise(int localGamerIndex, const rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status)
	{
		if (g_hooking.player_magnet)
		{
			bool found = false;

			for (const auto& id : g_hooking.session_advertisements)
			{
				if (std::equal(std::begin(id.m_Guid), std::end(id.m_Guid), std::begin(matchId->m_Guid), std::end(matchId->m_Guid)))
				{
					found = true;
					break;
				}
			}

			if (!found) // It's a legitimate advertisement, we're leaving the session.
			{
				status->m_StatusCode = rage::netStatus::PENDING;
				FiberPool::queueJob([=]
				{
					rage::netStatus legitimate_status{};
					OG(rage_rlScMatchmaking_Unadvertise)(localGamerIndex, matchId, &legitimate_status);

					legitimate_status.waitUntilDone();
					CommandPlayerMagnet::cleanup();

					status->m_StatusCode = rage::netStatus::SUCCEEDED;
				});

				return true;
			}

			// They don't take no for an answer (it throws an error if we say this operation failed).
			status->m_StatusCode = rage::netStatus::SUCCEEDED;
			return true;
		}

		return OG(rage_rlScMatchmaking_Unadvertise)(localGamerIndex, matchId, status);
	}

	// rage::netTransactor::SendResponse<rage::rlSessionDetailResponse>
	// Without advertisement multiplexing (rage_rlScMatchmaking_Advertise), this is useless.
	bool rage_netTransactor_SendResponse_rage_rlSessionDetailResponse(void* /* rage::netTransactor */ _this, const void* /* rage::netTransactionInfo */ txInfo, rage::rlSessionDetailResponse* msg)
	{
		if (g_hooking.player_magnet && msg->m_Response == 5) /* RESPONSE_HAVE_DETAILS */
		{
			msg->m_NumFilledPublicSlots = std::max(25u, msg->m_NumFilledPublicSlots);
		}
		
		return OG(rage_netTransactor_SendResponse_rage_rlSessionDetailResponse)(_this, txInfo, msg);
	}

	// NetworkBaseConfig::GetMatchmakingPool
	int __fastcall get_matchmaking_pool()
	{
		if (g_hooking.spoof_matchmaking_pool)
		{
			return g_hooking.spoofed_matchmaking_pool;
		}
		return OG(get_matchmaking_pool)();
	}

	uint32_t CExtraContentManager_GetCRC(CExtraContentManager* _this, unsigned int initValue)
	{
		if (g_hooking.spoof_extra_content_crc)
		{
			return g_hooking.spoofed_extra_content_crc;
		}
		if (g_hooking.fix_asset_hashes)
		{
			return g_hooking.legit_extra_content_crc;
		}
		return OG(CExtraContentManager_GetCRC)(_this, initValue);
	}

	void received_clone_create_ack(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, uint16_t object_id, uint32_t ack_code)
	{
		sync_src = sender->player_id;

		EventTally t(sync_src, FlowEvent::SYNCIN_ACK_CLONE_CREATE);
		if (!(t.reactions & REACTION_BLOCK))
		{
			__try
			{
				g_hooking.received_clone_create_ack_hook.getOriginal<decltype(&received_clone_create_ack)>()(mgr, sender, recipient, object_id, ack_code);
			}
			//__except (EXCEPTION_EXECUTE_HANDLER)
			__EXCEPTIONAL()
			{
				//t.add(FlowEvent::SE_CRASH, "CA");
			}
		}
		t.applyReactionsIn();
	}

	void received_clone_sync_ack(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, void* buffer, uint16_t object_id, uint32_t ack_code)
	{
		sync_src = sender->player_id;

		EventTally t(sync_src, FlowEvent::SYNCIN_ACK_CLONE_UPDATE);
		if (!(t.reactions & REACTION_BLOCK))
		{
			__try
			{
				g_hooking.received_clone_sync_ack_hook.getOriginal<decltype(&received_clone_sync_ack)>()(mgr, sender, recipient, buffer, object_id, ack_code);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				t.add(FlowEvent::SE_CRASH, "UA");
			}
		}
		t.applyReactionsIn();
	}

	void received_clone_remove_ack(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, uint16_t object_id, uint32_t ack_code)
	{
		sync_src = sender->player_id;

		EventTally t(sync_src, FlowEvent::SYNCIN_ACK_CLONE_DELETE);
		if (!(t.reactions & REACTION_BLOCK))
		{
			__try
			{
				g_hooking.received_clone_remove_ack_hook.getOriginal<decltype(&received_clone_remove_ack)>()(mgr, sender, recipient, object_id, ack_code);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				t.add(FlowEvent::SE_CRASH, "DA");
			}
		}
		t.applyReactionsIn();
	}

	static void logBlockedSync(floweventreaction_t& reactions, const EventLabel& event_name, rage::netObject* netObject, rage::NetworkObjectType object_type)
	{
		__try
		{
			if (auto tf = flow_event_reactions_to_logger_toast_flags(reactions))
			{
				std::string message = LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", event_name.getName()), FMT_ARG("player", sync_src.getName()));
				if (event_name.hasNonRawName())
				{
					message.append(": ").append(event_name.raw_name.getLocalisedUtf8());
				}
				message.append(": ").append(NetworkObjectType_toString(object_type));
				if (netObject != nullptr)
				{
					message.append("{id=").append(fmt::to_string(netObject->object_id)).append(", obj_type=").append(NetworkObjectType_names[netObject->object_type]).push_back('}');
				}
				Util::toast(std::move(message), tf);
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

#if DEBUG_SYNCTREE_BUFFER_OVERRUNS
	class SyncTreeStateSaver
	{
	private:
		std::vector<void*> data;
		size_t i;

		void save(rage::netSyncParentNode* parent)
		{
			for (auto node = parent->first_child; node != nullptr; node = node->next)
			{
				data.emplace_back(*reinterpret_cast<void**>(node));
				data.emplace_back(node->next);

				if (node->GetIsParentNode())
				{
					save(reinterpret_cast<rage::netSyncParentNode*>(node));
				}
			}
		}

		bool checkAndRestore(rage::netSyncParentNode* parent)
		{
			bool ret = false;
			for (auto node = parent->first_child; node != nullptr; node = node->next)
			{
				if (*reinterpret_cast<void**>(node) != data.at(i))
				{
					*reinterpret_cast<void**>(node) = data.at(i);
					ret = true;
				}
				if (++i >= data.size())
				{
					break;
				}

				if (node->next != data.at(i))
				{
					node->next = reinterpret_cast<rage::netSyncNodeBase*>(data.at(i));
					ret = true;
				}
				if (++i >= data.size())
				{
					break;
				}

				if (node->GetIsParentNode())
				{
					ret |= checkAndRestore(reinterpret_cast<rage::netSyncParentNode*>(node));
				}
			}
			return ret;
		}

	public:
		void save(rage::netSyncTree* tree)
		{
			data.clear();
			save(tree->root);
		}

		[[nodiscard]] bool checkAndRestore(rage::netSyncTree* tree)
		{
			if (data.empty())
			{
				return false;
			}
			i = 0;
			return checkAndRestore(tree->root);
		}
	};
	static SyncTreeStateSaver stss;
#endif

#if DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS
	class NetObjPlayerStateSaver
	{
	private:
		void* correct_vft_pointer = nullptr;

	public:
		void save()
		{
			SOUP_IF_UNLIKELY (correct_vft_pointer == nullptr)
			{
				for (int i = 0; i != MAX_PLAYERS; ++i)
				{
					if (auto cngp = (*pointers::network_player_mgr)->Players[i];
						cngp && cngp->player_info && cngp->player_info->m_ped && cngp->player_info->m_ped->m_net_object
						)
					{
						SOUP_ASSERT(*reinterpret_cast<void**>(cngp->player_info->m_ped->m_net_object) != nullptr);
						correct_vft_pointer = *reinterpret_cast<void**>(cngp->player_info->m_ped->m_net_object);
						break;
					}
				}
			}
		}

		[[nodiscard]] bool checkAndRestore() const
		{
			bool ret = false;
			SOUP_IF_LIKELY (correct_vft_pointer != nullptr)
			{
				for (int i = 0; i != MAX_PLAYERS; ++i)
				{
					if (auto cngp = (*pointers::network_player_mgr)->Players[i];
						cngp && cngp->player_info && cngp->player_info->m_ped && cngp->player_info->m_ped->m_net_object
						)
					{
						if (correct_vft_pointer != *reinterpret_cast<void**>(cngp->player_info->m_ped->m_net_object))
						{
							*reinterpret_cast<void**>(cngp->player_info->m_ped->m_net_object) = correct_vft_pointer;
							ret = true;
						}
					}
				}
			}
			return ret;
		}
	};
	static NetObjPlayerStateSaver nopss;
#endif

	static void processReceivedSyncTree(rage::datBitBuffer* buffer, rage::NetworkObjectType object_type, bool is_create, EventTally& t)
	{
		if (current_sync_tree != nullptr)
		{
			__try
			{
#if DEBUG_SYNCTREE_BUFFER_OVERRUNS
				if (stss.checkAndRestore(current_sync_tree))
				{
					Util::toast(soup::ObfusString("processReceivedSyncTree: stss had to restore data!").str(), TOAST_ALL);
				}
#endif
#if DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS
				if (nopss.checkAndRestore())
				{
					Util::toast(soup::ObfusString("processReceivedSyncTree: nopss had to restore data!").str(), TOAST_ALL);
				}
#endif
			}
			__EXCEPTIONAL()
			{
			}
		}
		__try
		{
			if (auto tf = flow_event_reactions_to_logger_toast_flags(t.reactions))
			{
				std::string message = LANG_FMT("PTX_NET_R_T_T", FMT_ARG("event", t.label.getName()), FMT_ARG("player", sync_src.getName()));
				if (t.label.hasNonRawName())
				{
					message.append(": ").append(t.label.raw_name.getLocalisedUtf8());
				}
				message.append(": ");
				if (sync_was_read)
				{
					message.append(current_sync_tree->dump(object_type, is_create));
				}
				else if (current_sync_tree != nullptr
					&& current_sync_tree->object != nullptr
					)
				{
					message.append(current_sync_tree->object->getTypeName()).append("{id=").append(fmt::to_string(current_sync_tree->object->object_id)).append("}");
				}
				else
				{
					buffer->seekStart();
					message.append(bitbufferToBitstring(*buffer));
					buffer->seekEnd();
				}
				Util::toast(std::move(message), tf);
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	static EventTally sync_tally{};
	static int aborting_sync = 0;

	// CNetworkObjectMgr::ProcessCloneCreateData
	void __fastcall received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, rage::NetworkObjectType object_type, uint16_t object_id, uint32_t sync_flags, rage::datBitBuffer* buffer, uint32_t timestamp)
	{
		THREAD_NAME("Sync Recv?");

		sync_src = sender->player_id;

		sync_tally = EventTally(sync_src, FlowEvent::SYNCIN_CLONE_CREATE);
		if (!(sync_tally.reactions & REACTION_BLOCK))
		{
			if (object_type >= NUM_NET_OBJ_TYPES)
			{
				if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_c1_when_coloading)
				{
					sync_tally.add(FlowEvent::SE_CRASH, "C1");
				}
				else
				{
					sync_tally.reactions |= REACTION_BLOCK;
				}
			}
		}
		rage::netObject* netObject = mgr->find_object_by_id(object_id, true);
		if (sync_tally.reactions & REACTION_BLOCK)
		{
			logBlockedSync(sync_tally.reactions, sync_tally.label, netObject, object_type);
		}
		else
		{
			current_sync_type = rage::SyncType::CREATE;
			sync_was_read = false;
			current_sync_tree = nullptr;
			current_create_object_type = object_type;
			current_create_object_id = object_id;
			__try
			{
				OG(received_clone_create)(mgr, sender, recipient, object_type, object_id, sync_flags, buffer, timestamp);
			}
			__EXCEPTIONAL_LOG_IF (aborting_sync == 0 && (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_c3_when_coloading))
			{
				buffer->seekEnd();
				if (aborting_sync != 0)
				{
					if (aborting_sync == 1)
					{
						sync_tally.add(FlowEvent::SE_CRASH, "CR");
					}
					aborting_sync = 0;
				}
				else
				{
					if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_c3_when_coloading)
					{
						sync_tally.add(FlowEvent::SE_CRASH, "C3");
						mgr->unregisterById(object_id);
					}
				}
			}
			processReceivedSyncTree(buffer, object_type, true, sync_tally);
		}
		sync_tally.applyReactionsIn();
	}

	// CNetworkObjectMgr::ProcessCloneSync
	__int64 __fastcall received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, rage::NetworkObjectType object_type, uint16_t object_id, rage::datBitBuffer* buffer, uint32_t unk, uint32_t timestamp)
	{
		sync_src = sender->player_id;

		__int64 ret = 1;
		sync_tally = EventTally(sync_src, FlowEvent::SYNCIN_CLONE_UPDATE);
		rage::netObject* netObject = mgr->find_object_by_id(object_id, true);
		if (!(sync_tally.reactions & REACTION_BLOCK))
		{
			if (object_type >= NUM_NET_OBJ_TYPES)
			{
				if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_u1_when_coloading)
				{
					sync_tally.add(FlowEvent::SE_CRASH, "U1");
				}
				else
				{
					sync_tally.reactions |= REACTION_BLOCK;
				}
			}

			if (netObject != nullptr && netObject->object_type != object_type)
			{
				if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_u2_when_coloading)
				{
					sync_tally.add(FlowEvent::SE_INVALID, "U2");
				}
				else
				{
					sync_tally.reactions |= REACTION_BLOCK;
				}
			}
		}
		if (sync_tally.reactions & REACTION_BLOCK)
		{
			logBlockedSync(sync_tally.reactions, sync_tally.label, netObject, object_type);
		}
		else
		{
			current_sync_type = rage::SyncType::UPDATE;
			sync_was_read = false;
			current_sync_tree = nullptr;
			__try
			{
				ret = g_hooking.received_clone_sync_hook.getOriginal<decltype(&received_clone_sync)>()(mgr, sender, recipient, object_type, object_id, buffer, unk, timestamp);
			}
			__EXCEPTIONAL_LOG_IF (aborting_sync == 0)
			{
				buffer->seekEnd();
				if (aborting_sync != 0)
				{
					sync_tally.add(FlowEvent::SE_CRASH, "UR");
					aborting_sync = 0;
				}
				else
				{
					sync_tally.add(FlowEvent::SE_CRASH, "U3");
				}
				ret = 0;
			}
			processReceivedSyncTree(buffer, object_type, false, sync_tally);
		}
		sync_tally.applyReactionsIn();
		return ret;
	}

	// CNetworkObjectMgr::CloneObject
	void __fastcall clone_pack(CNetworkObjectMgr* mgr, rage::netObject* netObject, CNetGamePlayer* recipient, rage::datBitBuffer* buffer)
	{
		sync_src = recipient->player_id;

		floweventreaction_t reactions = sync_src.getReactions(FlowEvent::SYNCOUT_CLONE_CREATE, false);
		if (!(reactions & REACTION_BLOCK))
		{
			__try
			{
				g_hooking.clone_pack_hook.getOriginal<decltype(&clone_pack)>()(mgr, netObject, recipient, buffer);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				buffer->seekEnd();

				__try
				{
					std::vector<flowevent_t> types{ FlowEvent::SYNCOUT_CLONE_CREATE, FlowEvent::SE_CRASH };
					sync_src.applyReactions(reactions, std::move(types), LIT(std::move(std::string(LANG_GET("PTX_CRSHE")).append(" (CP)"))));
				}
				__EXCEPTIONAL()
				{
				}
			}
		}
	}

#if false
	static bool process_clone_create(rage::netObjectMgrBase* mgr, rage::netObject* obj, CNetGamePlayer* player, rage::datBitBuffer* buffer)
	{
		if (g_hooking.nextgen_target != nullptr)
		{
			if (obj->object_type == NET_OBJ_TYPE_AUTOMOBILE)
			{
				auto ent = obj->GetEntity();
				if (ent->archetype->hash == ATSTRINGHASH("sentinel"))
				{
					return g_hooking.nextgen_target == player;
				}
			}
		}

		if (g_hooking.footlettuce_target != nullptr)
		{
			if (obj->object_type == NET_OBJ_TYPE_OBJECT)
			{
				if (g_hooking.footlettuce_target == player)
				{
					auto ent = obj->GetEntity();
					if (ent->archetype->hash == Hooking::footlettuce_model_from)
					{
						//Util::toast("sending sync", TC_SCRIPT_YIELDABLE);
						auto og_model_info = ent->archetype;
						unsigned int index;
						ent->archetype = OG(rage_fwArchetypeManager_GetArchetypeFromHashKey)(Hooking::footlettuce_model_to, &index);
						OG(send_clone_create)(mgr, obj, player, buffer);
						ent->archetype = og_model_info;
						g_hooking.footlettuce_target = nullptr;
						return false;
					}
				}
			}
		}

		return true;
	}

	// rage::netObjectMgrBase::CloneObject
	void __fastcall send_clone_create(rage::netObjectMgrBase* mgr, rage::netObject* obj, CNetGamePlayer* player, rage::datBitBuffer* buffer)
	{
		THREAD_NAME("Sync Send?");

		sync_src = player->player_id;

		EventTally t(sync_src, FlowEvent::SYNCOUT_CLONE_CREATE);

		bool can_send;
		SOUP_IF_UNLIKELY (g_hooking.outgoing_train_create_exclusive.isActive()
			&& obj->object_type == NET_OBJ_TYPE_TRAIN
			)
		{
			// Exclusive mode
			can_send = g_hooking.outgoing_train_create_exclusive.contains(player);
		}
		else
		{
			// Non-exclusive mode
			can_send = !(t.reactions & REACTION_BLOCK);
		}
		SOUP_IF_LIKELY (can_send)
		{
			__try
			{
				if (process_clone_create(mgr, obj, player, buffer))
				{
					OG(send_clone_create)(mgr, obj, player, buffer);
				}
			}
			__EXCEPTIONAL()
			{
				buffer->seekEnd();
				__try
				{
					if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_cs_when_coloading)
					{
						t.add(FlowEvent::SE_CRASH, "CS");
					}
				}
				__EXCEPTIONAL()
				{
				}
			}
		}
		__try
		{
			t.applyReactionsOut();
		}
		__EXCEPTIONAL()
		{
		}
	}
#endif

	// rage::netObjectMgrMessageHandler::SendCloneSync
	void __fastcall send_clone_sync(rage::netObjectMgrBase* mgr, CNetGamePlayer* player, rage::netObject* obj, __int64 a4, int16_t* a5, char a6)
	{
		sync_src = player->player_id;

		EventTally t(sync_src, FlowEvent::SYNCOUT_CLONE_UPDATE);

		bool can_send;
		SOUP_IF_UNLIKELY (g_hooking.outgoing_player_sync_exclusive.isActive()
			&& obj->object_type == NET_OBJ_TYPE_PLAYER
			)
		{
			// Exclusive mode
			can_send = g_hooking.outgoing_player_sync_exclusive.contains(player);
		}
		else
		{
			// Non-exclusive mode
			can_send = !(t.reactions & REACTION_BLOCK);
		}
		SOUP_IF_LIKELY (can_send)
		{
			__try
			{
				g_hooking.send_clone_sync_hook.getOriginal<decltype(&send_clone_sync)>()(mgr, player, obj, a4, a5, a6);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				__try
				{
					t.add(FlowEvent::SE_CRASH, "US");
				}
				__EXCEPTIONAL()
				{
				}
			}
		}
		__try
		{
			t.applyReactionsOut();
		}
		__EXCEPTIONAL()
		{
		}
	}

#if false
	// rage::netObjectMgrBase::RemoveClone
	bool __fastcall send_clone_remove(CNetworkObjectMgr* mgr, rage::netObject* obj, CNetGamePlayer* player, char a4)
	{
		sync_src = player->player_id;

		bool ret = true;
		EventTally t(sync_src, FlowEvent::SYNCOUT_CLONE_DELETE);
		if (!(t.reactions & REACTION_BLOCK))
		{
			__try
			{
				ret = g_hooking.send_clone_remove_hook.getOriginal<decltype(&send_clone_remove)>()(mgr, obj, player, a4);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				__try
				{
					t.add(FlowEvent::SE_CRASH, "DS");
				}
				__EXCEPTIONAL()
				{
				}
			}
		}
		__try
		{
			t.applyReactionsOut();
		}
		__EXCEPTIONAL()
		{
		}
		return ret;
	}
#endif

	void rage_netSyncTree_Read(rage::netSyncTree* tree, rage::SyncType sync_type, uint32_t sync_flags, rage::datBitBuffer* buffer, rage::netLoggingInterface* logger)
	{
		current_sync_tree = tree;
#if DEBUG_SYNCTREE_BUFFER_OVERRUNS
		stss.save(tree);
#endif
#if DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS
		nopss.save();
#endif
		sync_was_read = false;
		aborting_sync = 0;
		__try
		{
			OG(rage_netSyncTree_Read)(tree, sync_type, sync_flags, buffer, logger);
			sync_was_read = true;
		}
		__EXCEPTIONAL_LOG_IF (!ColoadMgr::coloading_with_cherax)
		{
			buffer->seekEnd();
		}
		if (!sync_was_read)
		{
			aborting_sync = 1;
			throw 0;
		}
		if (sync_type == rage::SyncType::CREATE
			&& is_session_started_and_transition_finished()
			//&& GET_MILLIS_SINCE(TpUtil::last_tp_time) >= 1000
			)
		{
			if (tree->extendsVehicle(current_create_object_type))
			{
				if (auto v = g_player_veh.getCVehicle(); v && v->archetype && v->m_net_object)
				{
					if (v->getModelAsObservedByOthers() == tree->getModel(current_create_object_type)
						&& static_cast<CVehicle*>(v)->m_randomSeed == static_cast<CVehicleSyncTree*>(tree)->creation_data.m_randomSeed
						//&& netSyncTree->getPos(current_create_object_type).distance(g_player_veh.getPos()) < 3.0f
						)
					{
						sync_tally.add(FlowEvent::SE_PVKICK);
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							aborting_sync = 2;
							throw 0;
						}
					}
				}
			}
		}
	}

#ifdef STAND_DEV
	bool __fastcall rage_netSyncDataNode_Read(rage::netSyncDataNode* _this, unsigned int serMode, unsigned int actFlags, rage::datBitBuffer* bitBuffer, rage::netLoggingInterface* pLog)
	{
		bool ret = OG(rage_netSyncDataNode_Read)(_this, serMode, actFlags, bitBuffer, pLog);
		__try
		{
#if DEBUG_SYNCTREE_BUFFER_OVERRUNS
			if (stss.checkAndRestore(current_sync_tree))
			{
				Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Read sync node {} ({}): stss had to restore data!").str()), _this->parent_tree->castAndGetNodeName((rage::NetworkObjectType)_this->parent_tree->object->object_type, _this), (void*)_this), TOAST_ALL);
			}
#endif
#if DEBUG_NET_OBJ_PLAYER_BUFFER_OVERRUNS
			if (nopss.checkAndRestore())
			{
				Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Read sync node {} ({}): nopss had to restore data!").str()), _this->parent_tree->castAndGetNodeName((rage::NetworkObjectType)_this->parent_tree->object->object_type, _this), (void*)_this), TOAST_ALL);
			}
#endif
		}
		__EXCEPTIONAL()
		{
		}
		return ret;
	}
#endif

	// rage::netSyncTree::CanApplyNodeData
	bool sync_can_apply(rage::netSyncTree* tree, rage::netObject* netObject)
		// called in received_clone_create, received_clone_sync, & CGiveControlEvent::handle_extra_data
	{
		current_sync_tree = tree;
		if (!sync_was_read)
		{
			return false;
		}

		if (current_sync_type == rage::SyncType::CREATE
			&& netObject->object_type == NET_OBJ_TYPE_PICKUP
			)
		{
			// Detect kick pickups (modded event instead of kick event because we might not be the target.)
			switch (static_cast<CPickupSyncTree*>(tree)->creation_data.model)
			{
			case ATSTRINGHASH("bkr_prop_coke_boxeddoll"):
			case ATSTRINGHASH("vw_prop_vw_colle_sasquatch"):
			case ATSTRINGHASH("vw_prop_vw_colle_beast"):
			case ATSTRINGHASH("vw_prop_vw_colle_rsrgeneric"):
			case ATSTRINGHASH("vw_prop_vw_colle_rsrcomm"):
			case ATSTRINGHASH("vw_prop_vw_colle_pogo"):
			case ATSTRINGHASH("vw_prop_vw_colle_prbubble"):
			case ATSTRINGHASH("vw_prop_vw_colle_imporage"):
			case ATSTRINGHASH("vw_prop_vw_colle_alien"):
				if (static_cast<CPickupSyncTree*>(tree)->creation_data.m_amount > 99)
				{
					sync_tally.add(FlowEvent::SE_INVALID, "TG");
					if (sync_tally.reactions & REACTION_BLOCK)
					{
						return false;
					}
				}
				break;

			case ATSTRINGHASH("vw_prop_vw_lux_card_01a"):
				if (static_cast<CPickupSyncTree*>(tree)->creation_data.m_amount > 53)
				{
					sync_tally.add(FlowEvent::SE_INVALID, "TG");
					if (sync_tally.reactions & REACTION_BLOCK)
					{
						return false;
					}
				}
				break;
			}

			for (uint32_t i = 0; i != static_cast<CPickupSyncTree*>(tree)->creation_data.m_numWeaponComponents; ++i)
			{
				// This would throw in CPickupCreationDataNode::CanApplyData, so it's important we check this before calling OG can_apply_data.
				if (!WeaponComponent::isValid(static_cast<CPickupSyncTree*>(tree)->creation_data.m_weaponComponents[i]))
				{
					sync_tally.add(FlowEvent::SE_CRASH, "TE");
					if (sync_tally.reactions & REACTION_BLOCK)
					{
						return false;
					}
				}
			}
		}

		__try
		{
			if (!OG(sync_can_apply)(tree, netObject))
			{
				return false;
			}
		}
		__EXCEPTIONAL()
		{
			if (!ColoadMgr::coloading_with_any_menu || !g_hooking.ignore_crash_t6_when_coloading)
			{
				sync_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, "T6");
			}
			return false;
		}

		if (current_sync_type == rage::SyncType::CONTROL)
		{
			sync_tally = EventTally(sync_src, FlowEvent::SYNCIN_CONTROL);
		}
		bool ret = process_tree_for_tally(tree, current_sync_type, netObject->object_type, netObject->object_id, netObject->GetEntity());
		if (current_sync_type == rage::SyncType::CONTROL)
		{
			sync_tally.applyReactionsIn();
		}
		return ret;
	}

	[[nodiscard]] static bool process_entity_spawn_or_teleport(rage::netSyncTree* tree, rage::NetworkObjectType object_type)
	{
		bool ret = true;

		if (object_type == NET_OBJ_TYPE_TRAIN)
		{
			const float fDistanceTopdown = tree->getPos(object_type).distanceTopdown(g_player_ent.getPos());

			// A carriage beneath our feet may be used to teleport us.
			if (fDistanceTopdown <= 5.0f)
			{
				sync_tally.add(FlowEvent::SE_UNUSUAL, "T0");
				if (sync_tally.reactions & REACTION_BLOCK)
				{
					ret = false;
				}
			}
		}

		return ret;
	}

	bool process_tree_for_tally(rage::netSyncTree* tree, rage::SyncType sync_type, rage::NetworkObjectType object_type, rage::ObjectId object_id, CEntity* ent)
	{
		bool ret = true;

		hash_t model = 0;
		if (sync_type == rage::SyncType::CREATE)
		{
			model = tree->getModel(object_type);

			const float fDistanceTopdown = tree->getPos(object_type).distanceTopdown(g_player_ent.getPos());

			if (fDistanceTopdown < 70.0f)
			{
				// Why this exists:
				// - "Launch Player" spawns invisible vehicle under the player and applies upwards force to it.
				// - "Hijack Vehicle" spawns invisible ped that will pull the user out of their vehicle.
				// There shouldn't be anything invisible spawned near the user anyway.
				if (!tree->isVisible(object_type)
					&& object_type != NET_OBJ_TYPE_PLAYER // Players loading in next to us
					&& object_type != NET_OBJ_TYPE_PICKUP // Dead players dropping pickups
					&& !AbstractModel(model).isValidParachute() // Players parachuting
					&& (object_type != NET_OBJ_TYPE_AUTOMOBILE || tree->getPos(object_type).distance(v3{ 1561.4769f, 416.95386f, -55.171387f }) > 10.0f) // Being passenger in toreador that is being driven into kosatka
					&& object_type != NET_OBJ_TYPE_HELI // Leaving office with helicopter
#ifdef STAND_DEV
					&& (is_session_freeroam() || object_type != NET_OBJ_TYPE_OBJECT)
					&& model != ATSTRINGHASH("h4_prop_h4_ante_on_01a") // Freemode collectable
					&& model != ATSTRINGHASH("tr_prop_tr_usb_drive_02a") // Freemode collectable
					&& model != ATSTRINGHASH("reh_prop_reh_bag_weed_01a") // Freemode collectable (LD Organics)
					&& model != ATSTRINGHASH("prop_amb_beer_bottle")
					&& model != ATSTRINGHASH("ch_prop_casino_drone_01a") // Nano drone
					&& model != ATSTRINGHASH("ex_p_ex_tumbler_03_s")
					&& model != ATSTRINGHASH("ex_prop_tv_settop_remote") // Starting "Don't Cross The Line" in CEO Office
					&& model != ATSTRINGHASH("lux_p_champ_flute_s") // Player drinking champaigne in our luxor deluxe
					&& model != ATSTRINGHASH("lux_p_pour_champagne_luxe") // Player drinking champaigne in our luxor deluxe
					&& model != ATSTRINGHASH("p_wine_glass_s") // Agency
					&& model != ATSTRINGHASH("p_pour_wine_s") // Agency
					&& model != ATSTRINGHASH("sf_prop_sf_usb_drive_01a") // Agency
					&& model != ATSTRINGHASH("prop_boombox_01") // Yacht
					&& model != ATSTRINGHASH("vw_prop_vw_key_card_01a") // Casino Heist Security Passes Setup
					&& model != ATSTRINGHASH("ch_prop_ch_heist_drill") // Arcade Drilling Practice
					&& model != ATSTRINGHASH("hei_prop_hei_drill_hole") // Arcade Drilling Practice
					&& model != ATSTRINGHASH("prop_choc_ego") // Store Robbery
					&& model != ATSTRINGHASH("prop_choc_meto") // Store Robbery
					&& model != ATSTRINGHASH("prop_choc_pq") // Store Robbery
					&& model != ATSTRINGHASH("prop_ecola_can") // Store Robbery
					&& model != ATSTRINGHASH("prop_ld_fags_01") // Store Robbery
#else
					&& object_type != NET_OBJ_TYPE_OBJECT
#endif
					&& model != ATSTRINGHASH("trailerlarge") // Trailers near the docks spawn invisible
					)
				{
					if (fDistanceTopdown < 10.0f)
					{
						sync_tally.add(FlowEvent::SE_UNUSUAL, "T7");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
					else if (object_type == NET_OBJ_TYPE_PED)
					{
						auto ped_base_tree = static_cast<CPedSyncTreeBase*>(tree);
						if (!ped_base_tree->task_data.applies
							|| ped_base_tree->task_data.primary_task == CTaskTypes::MAX_NUM_TASK_TYPES
							)
						{
							// Next time, they will sync the primary_task as well, allowing us to catch "Hijack Vehicle" with T8.
							if (!resync_requested.contains(object_id))
							{
#ifdef STAND_DEBUG
								Util::toast("T7: Ped is sus, requesting resync");
#endif
								ret = false;
								resync_requested.emplace(object_id);
							}
						}
					}
				}

				switch (object_type)
				{
				case NET_OBJ_TYPE_AUTOMOBILE:
					if (auto cmd = sync_src.getCommand())
					{
						if (cmd->create_automobile_ratelimit.canRequest())
						{
							if (GET_MILLIS_SINCE(TpUtil::last_tp_time) > 10000)
							{
								cmd->create_automobile_ratelimit.addRequest();
							}
						}
						else
						{
							sync_tally.add(FlowEvent::SE_UNUSUAL, "T0");
							if (sync_tally.reactions & REACTION_BLOCK)
							{
								ret = false;
							}
						}
					}
					break;

				case NET_OBJ_TYPE_PED:
					switch (model)
					{
					case ATSTRINGHASH("s_m_y_casino_01"):
					case ATSTRINGHASH("s_f_y_casino_01"):
						break;

					default:
						if (is_session_freeroam()
							// Still run this detection in activity sessions, but with exclusions:
							|| (model != ATSTRINGHASH("a_c_panther")
								&& model != ATSTRINGHASH("s_m_y_swat_01")
								)
							)
						{
							if (auto cmd = sync_src.getCommand())
							{
								const bool b1 = cmd->create_ped_ratelimit.canRequest();
								const bool b2 = (fDistanceTopdown > 10.0f || cmd->create_ped_ratelimit_upclose.canRequest());
								if (GET_MILLIS_SINCE(TpUtil::last_tp_time) > 10000)
								{
									if (b1)
									{
										cmd->create_ped_ratelimit.addRequest();
									}
									if (b2 && fDistanceTopdown <= 10.0f)
									{
										cmd->create_ped_ratelimit_upclose.addRequest();
									}
								}
								if (!b1 || !b2)
								{
									sync_tally.add(FlowEvent::SE_UNUSUAL, "T0");
									if (sync_tally.reactions & REACTION_BLOCK)
									{
										ret = false;
									}
								}
							}
						}
						break;
					}
					break;

				case NET_OBJ_TYPE_BIKE:
					if (auto cmd = sync_src.getCommand())
					{
						if (cmd->create_bike_ratelimit.canRequest())
						{
							if (GET_MILLIS_SINCE(TpUtil::last_tp_time) > 10000)
							{
								cmd->create_bike_ratelimit.addRequest();
							}
						}
						else
						{
							sync_tally.add(FlowEvent::SE_UNUSUAL, "T0");
							if (sync_tally.reactions & REACTION_BLOCK)
							{
								ret = false;
							}
						}
					}
					break;

					// Having these at a slightly higher rate limit because entering someone's hangar might cause a higher amount of plane syncs
				case NET_OBJ_TYPE_PLANE:
				case NET_OBJ_TYPE_HELI:
					if (auto cmd = sync_src.getCommand())
					{
						if (cmd->create_aircraft_ratelimit.canRequest())
						{
							cmd->create_aircraft_ratelimit.addRequest();
						}
						else
						{
							sync_tally.add(FlowEvent::SE_UNUSUAL, "T1");
							if (sync_tally.reactions & REACTION_BLOCK)
							{
								ret = false;
							}
						}
					}
					break;

				case NET_OBJ_TYPE_OBJECT:
					switch (model)
					{
						// Multiple interiors
					case ATSTRINGHASH("p_cs_lighter_01"):
					case ATSTRINGHASH("p_cs_shot_glass_2_s"):
					case ATSTRINGHASH("p_w_grass_gls_s"):
					case ATSTRINGHASH("p_whiskey_bottle_s"):
					case ATSTRINGHASH("prop_wine_bot_01"):
					case ATSTRINGHASH("prop_wine_glass"):
						// Apartment
					case ATSTRINGHASH("apa_p_mp_door_apartfrt_door"):
					case ATSTRINGHASH("apa_p_mp_door_apartfrt_door_black"):
					case ATSTRINGHASH("apa_p_mp_door_mpa2_frnt"):
					case ATSTRINGHASH("apa_p_mp_door_stilt_doorfrnt"):
					case ATSTRINGHASH("hei_prop_heist_tumbler_empty"):
					case ATSTRINGHASH("p_tumbler_01_s"):
					case ATSTRINGHASH("prop_bong_01"):
					case ATSTRINGHASH("prop_boombox_01"):
					case ATSTRINGHASH("v_ilev_garageliftdoor"):
						// Casino Penthouse
					case ATSTRINGHASH("ba_prop_battle_decanter_02_s"):
					case ATSTRINGHASH("ba_prop_battle_decanter_03_s"):
					case ATSTRINGHASH("ba_prop_battle_whiskey_opaque_s"):
					case ATSTRINGHASH("ex_p_ex_tumbler_01_empty"):
					case ATSTRINGHASH("ex_p_ex_tumbler_01_s"):
					case ATSTRINGHASH("ex_p_ex_tumbler_02_empty"):
					case ATSTRINGHASH("ex_p_ex_tumbler_02_s"):
					case ATSTRINGHASH("ex_p_ex_tumbler_03_empty"):
					case ATSTRINGHASH("ex_p_ex_tumbler_03_s"):
					case ATSTRINGHASH("ex_prop_exec_ashtray_01"):
					case ATSTRINGHASH("lux_prop_cigar_01_luxe"):
					case ATSTRINGHASH("ng_proc_cigarette01a"):
					case ATSTRINGHASH("p_cs_scissors_s"):
					case ATSTRINGHASH("p_pour_wine_s"):
					case ATSTRINGHASH("p_wine_glass_s"):
					case ATSTRINGHASH("vw_prop_casino_phone_01a"):
					case ATSTRINGHASH("vw_prop_vw_offchair_01"):
					case ATSTRINGHASH("vw_prop_vw_safedoor_office2a_l"):
					case ATSTRINGHASH("vw_prop_vw_safedoor_office2a_r"):
						// Arcade
					case ATSTRINGHASH("ch_prop_arcade_race_bike_02a"):
					case ATSTRINGHASH("ch_prop_arcade_race_car_01a"):
					case ATSTRINGHASH("ch_prop_arcade_race_car_01b"):
					case ATSTRINGHASH("ch_prop_ch_arcade_safe_door"):
					case ATSTRINGHASH("prop_npc_phone"):
					case ATSTRINGHASH("sum_prop_arcade_str_bar_01a"):
					case ATSTRINGHASH("sum_prop_arcade_str_lightoff"):
					case ATSTRINGHASH("sum_prop_arcade_str_lighton"):
					case ATSTRINGHASH("sum_prop_arcade_strength_ham_01a"):
					case ATSTRINGHASH("tr_prop_tr_wpncamhedz_01a"):
						// airfreight cargo dropped from parachute
					case ATSTRINGHASH("p_cargo_chute_s"):
						// Starting "Don't Cross The Line" in CEO Office
					case ATSTRINGHASH("ex_prop_tv_settop_remote"):
						// LS Customs
					case ATSTRINGHASH("tr_prop_tr_chair_01a"):
						break;

					default:
						if (is_session_freeroam()) // Cayo Perico Heist: hei_prop_hei_keypad_01, hei_prop_hei_keypad_01, hei_prop_hei_keypad_01, hei_prop_hei_keypad_01, hei_prop_hei_keypad_01, hei_prop_hei_keypad_01, h4_prop_h4_painting_01a, hei_prop_hei_keypad_01, hei_prop_hei_keypad_01, h4_prop_h4_painting_01b, h4_prop_h4_painting_01c, h4_prop_h4_painting_01e, h4_prop_h4_painting_01f, h4_prop_h4_painting_01g, prop_amanda_note_01, prop_amanda_note_01, w_pi_singleshoth4, w_sg_pumpshotgunh4
						{
							if (auto cmd = sync_src.getCommand())
							{
								if (cmd->create_object_ratelimit.canRequest())
								{
									cmd->create_object_ratelimit.addRequest();
								}
								else
								{
									sync_tally.add(FlowEvent::SE_UNUSUAL, "T1");
									if (sync_tally.reactions & REACTION_BLOCK)
									{
										ret = false;
									}
								}
							}
						}
						break;
					}
					break;

				case NET_OBJ_TYPE_TRAIN:
					if (auto cmd = sync_src.getCommand())
					{
						if (cmd->uncommon_sync_ratelimit.canRequest())
						{
							cmd->uncommon_sync_ratelimit.addRequest();
						}
						else
						{
							sync_tally.add(FlowEvent::SE_UNUSUAL, "T1");
							if (sync_tally.reactions & REACTION_BLOCK)
							{
								ret = false;
							}
						}
					}
					break;
				}

				if (!process_entity_spawn_or_teleport(tree, object_type))
				{
					ret = false;
				}
			}

			switch (object_type)
			{
			case NET_OBJ_TYPE_BOAT:
				if (auto cmd = sync_src.getCommand())
				{
					if (cmd->create_boat_ratelimit.canRequest())
					{
						cmd->create_boat_ratelimit.addRequest();
					}
					else
					{
						sync_tally.add(FlowEvent::SE_UNUSUAL, "T1");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
				}
				break;

			case NET_OBJ_TYPE_SUBMARINE:
				if (auto cmd = sync_src.getCommand())
				{
					if (cmd->uncommon_sync_ratelimit.canRequest())
					{
						cmd->uncommon_sync_ratelimit.addRequest();
					}
					else
					{
						sync_tally.add(FlowEvent::SE_UNUSUAL, "T1");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
				}
				break;
			}

			if (model != 0)
			{
				if (!is_valid_model(object_type, model))
				{
					if (on_invalid_model_sync(object_type, model))
					{
						ret = false;
					}
				}
				else
				{
					if (check_custom_model_reactions(model))
					{
						ret = false;
					}
					else
					{
						if (object_type == NET_OBJ_TYPE_PED)
						{
							if (AbstractModel(model).isHiResPed())
							{
								if (auto cmd = sync_src.getCommand())
								{
									if (cmd->hiresped_ratelimit.canRequest())
									{
										cmd->hiresped_ratelimit.addRequest();
									}
									else
									{
										sync_tally.add(FlowEvent::SE_INVALID, "TC");
										if (sync_tally.reactions & REACTION_BLOCK)
										{
											ret = false;
										}
									}
								}
							}
						}
						else if (object_type == NET_OBJ_TYPE_OBJECT)
						{
							auto mi = g_hooking.getModelInfo(model);
							if (mi == nullptr
								|| (mi->flags & 0x10) == 0
								|| mi->getAttribute() == 14 // MODEL_SINGLE_AXIS_ROTATION to catch prop_roofvent_06a
								)
							{
								if (model != 0x2B3ACD6A) // we don't wanna trigger if legits destroy a fence
								{
									sync_tally.add(FlowEvent::SYNCIN_WOS);
									if (sync_tally.reactions & REACTION_BLOCK)
									{
										ret = false;
									}
								}
							}
						}
						else if (object_type == NET_OBJ_TYPE_BOAT)
						{
							if (auto cmd = sync_src.getCommand())
							{
								const auto this_boat_pos = tree->getPos(object_type);

								// A lot of boats inside of each other causes the game to lag on physics for a bit.
								// They don't even have to be near the user, just anywhere in the world.
								// See scripts/boat_lag.pluto
								if (cmd->last_boat_create_id != object_id
									&& cmd->last_boat_create_pos.distance(this_boat_pos) < 1.0f
									)
								{
									sync_tally.add(FlowEvent::SE_INVALID, "T7");
									if (sync_tally.reactions & REACTION_BLOCK)
									{
										ret = false;
									}
								}

								cmd->last_boat_create_id = object_id;
								cmd->last_boat_create_pos = this_boat_pos;
							}
						}

						switch (model)
						{
						case ATSTRINGHASH("mule"):
						case ATSTRINGHASH("mule2"):
						case ATSTRINGHASH("mule3"):
						case ATSTRINGHASH("mule4"):
						case ATSTRINGHASH("mule5"):
						case ATSTRINGHASH("boxville"):
						case ATSTRINGHASH("boxville2"):
						case ATSTRINGHASH("boxville3"):
						case ATSTRINGHASH("boxville4"):
						case ATSTRINGHASH("boxville5"):
						case ATSTRINGHASH("benson"):
						case ATSTRINGHASH("benson2"):
						case ATSTRINGHASH("prop_gascage01"):
						case ATSTRINGHASH("hei_prop_heist_apecrate"):
						case ATSTRINGHASH("prop_feeder1_cr"):
							if (tree->getPos(object_type).distance(g_player_ent.getPos()) < 2.5f)
							{
								sync_tally.add(FlowEvent::SYNCIN_CAGE);
								if (sync_tally.reactions & REACTION_BLOCK)
								{
									ret = false;
								}
							}
							break;

						case ATSTRINGHASH("p_cablecar_s"):
						case ATSTRINGHASH("prop_gold_cont_01"):
						case ATSTRINGHASH("prop_rub_cage01a"):
						case ATSTRINGHASH("prop_fnclink_03e"):
						case ATSTRINGHASH("prop_container_05a"):
						case ATSTRINGHASH("prop_container_ld_pu"):
						case ATSTRINGHASH("prop_test_elevator"):
						case ATSTRINGHASH("prop_fnclink_05crnr1"):
						case ATSTRINGHASH("prop_mb_hesco_06"):
							if (tree->getPos(object_type).distance(g_player_ent.getPos()) < 7.0f)
							{
								sync_tally.add(FlowEvent::SYNCIN_CAGE);
								if (sync_tally.reactions & REACTION_BLOCK)
								{
									ret = false;
								}
							}
							break;

						case ATSTRINGHASH("stt_prop_stunt_tube_s"):
						case ATSTRINGHASH("sr_prop_spec_tube_xxs_04a"):
							if (tree->getPos(object_type).distance(g_player_ent.getPos()) < 100.0f)
							{
								sync_tally.add(FlowEvent::SYNCIN_CAGE);
								if (sync_tally.reactions & REACTION_BLOCK)
								{
									ret = false;
								}
							}
							break;

						case ATSTRINGHASH("apa_mp_h_str_avunitl_01_b"):
							// This can be used to glitch the user's vehicle when spawned close enough
							if (tree->getPos(object_type).distance(g_player_ent.getPos()) < 20.0f)
							{
								sync_tally.add(FlowEvent::SE_INVALID, "T9");
								if (sync_tally.reactions & REACTION_BLOCK)
								{
									ret = false;
								}
							}
							break;

						case ATSTRINGHASH("stt_prop_track_speedup"):
						case ATSTRINGHASH("stt_prop_track_speedup_t1"):
						case ATSTRINGHASH("stt_prop_track_speedup_t2"):
						case ATSTRINGHASH("stt_prop_track_slowdown"):
						case ATSTRINGHASH("stt_prop_track_slowdown_t1"):
						case ATSTRINGHASH("stt_prop_track_slowdown_t2"):
							if (tree->getPos(object_type).distance(g_player_ent.getPos()) < 10.0f)
							{
								sync_tally.add(FlowEvent::SE_INVALID, "T9");
								if (sync_tally.reactions & REACTION_BLOCK)
								{
									ret = false;
								}
							}
							break;
						}
					}
				}
			}
		}
		else // if (sync_type == rage::SyncType::UPDATE)
		{
			if (ent)
			{
				if (ent->archetype)
				{
					model = ent->archetype->hash;
				}

				if (ent->matrix.getTranslate().distance(tree->getPos(object_type)) > 5.0f)
				{
					if (!process_entity_spawn_or_teleport(tree, object_type))
					{
						ret = false;
					}
				}
			}
		}

		if (rage::netSyncTree::extendsDynamicEntityBase(object_type))
		{
			auto* game_state = &static_cast<CDynamicEntitySyncTreeBase*>(tree)->dynamic_entity_game_state;
			if (game_state->applies)
			{
				rage::fwInteriorLocation intloc;
				intloc.as_uint32 = game_state->m_InteriorProxyLoc;
				// CInteriorProxy::GetFromLocation compares m_interiorIndex to pool size as a signed integer, so negative numbers would be permissible by the conditional.
				// rage::fwInteriorLocation::IsValid does special-case -1 as invalid, but it also permits all other negative numbers.
				// So, if this were allowed, CInteriorProxy::GetFromLocation could return a garbage pointer, and callers would go with it because it's not nullptr.
				if (intloc.as_interiorLocation.m_interiorIndex < -1)
				{
					sync_tally.add(FlowEvent::SE_CRASH, "TC");
					if (sync_tally.reactions & REACTION_BLOCK)
					{
						ret = false;
					}
				}
			}
		}

		if (object_type == NET_OBJ_TYPE_PLAYER || object_type == NET_OBJ_TYPE_PED)
		{
			const auto ped_base_tree = reinterpret_cast<CPedSyncTreeBase*>(tree);

			if (ped_base_tree->ped_game_state.applies)
			{
				// Setting weapons as gadgets may be used as a way to sync equipping them
				for (uint32_t i = 0; i != ped_base_tree->ped_game_state.m_numGadgets; ++i)
				{
					if (ped_base_tree->ped_game_state.m_equippedGadgets[i] != ATSTRINGHASH("GADGET_PARACHUTE")
						&& ped_base_tree->ped_game_state.m_equippedGadgets[i] != ATSTRINGHASH("GADGET_NIGHTVISION")
						)
					{
						sync_tally.add(FlowEvent::SE_INVALID, "T5");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ped_base_tree->ped_game_state.m_numGadgets = 0;
						}
						break;
					}
				}

				// If this ped has a weapon equipped, it should have the proper bones.
				// I've decided to exclude players from this because they can transform to animals and use guns.
				// While it is technically correct to call it a modded event, it is very annoying and not very useful.
				// Stand should hopefully fix the related crashes anyway.
				// As a bonus, this also blocks "Send Cris Formage" from 2T1.
				if (object_type != NET_OBJ_TYPE_PLAYER)
				{
					const auto weapon_hash = ped_base_tree->ped_game_state.m_weapon;
					if (weapon_hash != 0
						&& weapon_hash != ATSTRINGHASH("WEAPON_UNARMED")
						&& Weapon::isValidHash(weapon_hash) // Animals seem to have weird weapons like weapon_animal_retriever, weapon_rabbit, etc. when they should be unarmed.
						)
					{
						if (!AbstractModel(model).canUseProjectiles()
							|| AbstractModel(model).isCsPed()
							|| !AbstractModel(model).canUseWeapons()
							)
						{
							//Util::toast(fmt::format("{} has weapon ({}) without weapon bones", joaatToString(model), joaatToString(ped_base_tree->ped_game_state.m_weapon)));
							if (ret) // avoid shadowing crash event, e.g. T8
							{
								sync_tally.add(FlowEvent::SE_INVALID, "T6");
								if (sync_tally.reactions & REACTION_BLOCK)
								{
									ret = false;
								}
							}
						}
					}
				}

				if (ped_base_tree->ped_game_state.m_mountID != NETWORK_INVALID_OBJECT_ID)
				{
					// There are no mounts in the game.
					// This just straight up causes nullptr dereferences and shit because the relevant logic is (most) disabled.
					sync_tally.add(FlowEvent::SE_CRASH, "TA");
					if (sync_tally.reactions & REACTION_BLOCK)
					{
						ret = false;
					}
				}
			}

			if (ped_base_tree->task_data.applies)
			{
#if false // There seem to be instances where NPCs have no task even after resync
				if (ret
					&& object_type == NET_OBJ_TYPE_PED
					&& sync_type == rage::SyncType::CREATE
					&& ped_base_tree->task_data.primary_task == CTaskTypes::MAX_NUM_TASK_TYPES
					)
				{
					if (!resync_requested.contains(object_id))
					{
						Util::toast("Spawned ped has no task, requesting resync");
						resync_requested.emplace(object_id);
						ret = false;
					}
					else
					{
						Util::toast("Spawned ped still has no task after resync");
					}
				}
#endif
				if (ped_base_tree->task_data.primary_task == CTaskTypes::TASK_UNALERTED)
				{
					if (object_type == NET_OBJ_TYPE_PLAYER)
					{
						sync_tally.add(FlowEvent::SE_INVALID, "T3");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
				}
				else if (ped_base_tree->task_data.primary_task == CTaskTypes::TASK_ENTER_VEHICLE)
				{
					if (object_type == NET_OBJ_TYPE_PED
						&& sync_type == rage::SyncType::CREATE
						&& !tree->isVisible(object_type)
						//&& model != ATSTRINGHASH("s_m_y_cop_01")
						//&& model != ATSTRINGHASH("s_m_y_sheriff_01")
						//&& model != ATSTRINGHASH("s_m_y_waretech_01")
						)
					{
						sync_tally.add(FlowEvent::SE_INVALID, "T8");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
				}
				else if (ped_base_tree->task_data.primary_task == CTaskTypes::TASK_AIM_AND_THROW_PROJECTILE)
				{
					if (!AbstractModel(model).canUseProjectiles())
					{
						sync_tally.add(FlowEvent::SE_CRASH, "TB");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
				}
			}

			if (object_type == NET_OBJ_TYPE_PLAYER)
			{
				auto ped = reinterpret_cast<CPed*>(ent);

				const auto player_tree = reinterpret_cast<CPlayerSyncTree*>(tree);
				if (player_tree->appearance_data.applies)
				{
					// Keep track of real model for "Modded Character Model"
					if (auto cmd = sync_src.getCommand())
					{
						cmd->syncing_as_model = player_tree->appearance_data.m_NewModelHash;
					}

					if (g_hooking.block_player_model_swaps
						&& !g_hooking.block_player_model_swaps_excludes.isExcluded(sync_src)
						&& ped
						&& ped->archetype
						&& ped->archetype->hash != player_tree->appearance_data.m_NewModelHash
						)
					{
						if (AbstractModel(ped->archetype->hash).isPlayableOnlineCharacter())
						{
							player_tree->appearance_data.m_NewModelHash = ped->archetype->hash;
						}
						else
						{
							player_tree->appearance_data.m_NewModelHash = ATSTRINGHASH("mp_m_freemode_01");
						}
					}

					switch (player_tree->appearance_data.m_NewModelHash)
					{
					case ATSTRINGHASH("cs_solomon"):
					case ATSTRINGHASH("cs_josh"):
					case ATSTRINGHASH("cs_lazlow"):
						g_hooking.schizo_sync_blamer.setBlame(sync_src);
						break;
					}

					if (!is_valid_model_ped(player_tree->appearance_data.m_NewModelHash))
					{
						if (on_invalid_model_sync(NET_OBJ_TYPE_PLAYER, player_tree->appearance_data.m_NewModelHash))
						{
							if (ped && ped->archetype)
							{
								player_tree->appearance_data.m_NewModelHash = ped->archetype->hash;
							}
							else
							{
								ret = false;
							}
						}
						else
						{
							model = player_tree->appearance_data.m_NewModelHash;
						}
					}
					else
					{
						if (check_custom_model_reactions(player_tree->appearance_data.m_NewModelHash))
						{
							if (ped && ped->archetype)
							{
								player_tree->appearance_data.m_NewModelHash = ped->archetype->hash;
							}
							else
							{
								ret = false;
							}
						}
						else
						{
							model = player_tree->appearance_data.m_NewModelHash;
						}
					}

					if (player_tree->appearance_data.m_HasHeadBlendData
						&& !AbstractModel(player_tree->appearance_data.m_NewModelHash).isOnlineCharacter()
						)
					{
						// Head blend data on a non-online ped causes issues with the skeleton/bones.
						// This should be a crash event, but shitty model spoof options also cause this.
						sync_tally.add(FlowEvent::SE_INVALID, "TB");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							player_tree->appearance_data.m_HasHeadBlendData = false;
						}
					}

					// The devs fucked up here; they use m_DictHash to store the streaming request index.
					// That's not how you fucking network an animation.
					// This may lead to nullptr derefs, possibly worse.
					if (player_tree->appearance_data.m_secondaryPartialAnim.m_PhoneSecondary)
					{
						player_tree->appearance_data.m_secondaryPartialAnim.m_PhoneSecondary = false;
						sync_tally.add(FlowEvent::SE_UNUSUAL, "TH");
					}
				}

				if (player_tree->physical_game_state.applies)
				{
					if (auto cmd = sync_src.getCommand())
					{
						cmd->syncing_as_visible = player_tree->physical_game_state.m_isVisible;
					}
				}

				if (player_tree->appearance_data.applies
					&& player_tree->appearance_data.commons.isInvalid()
					)
				{
					sync_tally.add(FlowEvent::SE_CRASH, "T7");
					ret = false;
				}

				if (player_tree->player_camera.applies)
				{
					// Counteract this garbage YimMenu does: https://github.com/YimMenu/YimMenu/blob/4046640c1e2e9f7fdd6d51895020889d98ca30b6/src/hooks/spoofing/write_node_data.cpp#L112
					SOUP_IF_UNLIKELY (player_tree->player_camera.rotUpDown < 0.0f || player_tree->player_camera.rotUpDown > M_PI
						|| player_tree->player_camera.rotLeftRight < 0.0f || player_tree->player_camera.rotLeftRight > M_TAU
						)
					{
						if (is_session_started_and_transition_finished())
						{
#ifdef STAND_DEBUG // They don't need to know :)
							sync_src.triggerDetection(FlowEvent::MOD_MISC, Codename("M4").toString());
#endif
							player_tree->player_camera.absolute_pos.x -= 50.0f;
							player_tree->player_camera.absolute_pos.y += 50.0f;
						}
					}
				}

				if (player_tree->player_game_state.applies
					&& player_tree->player_game_state.m_bUseExtendedPopulationRange
					)
				{
					// This can be used to spectate without the camera being nowhere near the target.
					//Util::toast(fmt::format("{} using extended population range, curbing that", sync_src.getName()));
					player_tree->player_game_state.m_bUseExtendedPopulationRange = false;
				}

				if (player_tree->extended_game_state.applies)
				{
					if (auto cmd = sync_src.getCommand())
					{
						//cmd->has_waypoint = player_tree->extended_game_state.m_bHasActiveWaypoint;
						//cmd->is_owner_of_waypoint = player_tree->extended_game_state.m_bOwnsWaypoint;
						cmd->has_waypoint = (player_tree->extended_game_state.m_bHasActiveWaypoint && player_tree->extended_game_state.m_bOwnsWaypoint);
						cmd->waypoint_x = player_tree->extended_game_state.m_fxWaypoint;
						cmd->waypoint_y = player_tree->extended_game_state.m_fyWaypoint;
					}
				}
			}
			else //if (object_type == NET_OBJ_TYPE_PED)
			{
				const auto ped_tree = reinterpret_cast<CPedSyncTree*>(tree);

				if (ped_tree->appearance_data.applies
					&& ped_tree->appearance_data.commons.isInvalid()
					)
				{
					sync_tally.add(FlowEvent::SE_CRASH, "T7");
					ret = false;
				}

				/*if (sync_type == rage::SyncType::CREATE)
				{
					if (ped_tree->creation_data.m_inVehicle && g_player_veh.isNetObjId(ped_tree->creation_data.m_vehicleID))
					{
						Util::toast("Ped created in our vehicle", TOAST_ALL);
					}
				}*/

				if (ped_base_tree->ped_game_state.applies
					&& ped_base_tree->ped_game_state.m_inVehicle
					&& g_player_veh.isNetObjId(ped_base_tree->ped_game_state.m_vehicleID)
					)
				{
					//Util::toast("Ped is now on our vehicle", TOAST_ALL);
					if (ent)
					{
						if (auto att_ext = ent->GetAttachmentExtension(); att_ext && att_ext->m_pAttachChild)
						{
							//Util::toast("Ped has attachment(s)", TOAST_ALL);
							sync_tally.add(FlowEvent::SE_UNUSUAL, "T2");
							if (sync_tally.reactions & REACTION_BLOCK)
							{
								ret = false;
							}
						}
					}
				}
			}
		}
		else if (rage::netSyncTree::extendsVehicle(object_type))
		{
			auto veh_tree = reinterpret_cast<CVehicleSyncTree*>(tree);

			if (veh_tree->m_vehicleProximityMigrationNode.applies
				&& !g_player_veh.isNetObjId(object_id)
				)
			{
				const auto user_net_obj_id = g_player_ped.getNetObjId();
				for (uint32_t i = 0; i != veh_tree->m_vehicleProximityMigrationNode.m_maxOccupants; ++i)
				{
					if (veh_tree->m_vehicleProximityMigrationNode.m_hasOccupant[i]
						&& veh_tree->m_vehicleProximityMigrationNode.m_occupantID[i] == user_net_obj_id
						)
					{
						// Not the vehicle we're currently in, but data node claims we are.
						sync_tally.add(FlowEvent::SE_INVALID, "T4");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							veh_tree->m_vehicleProximityMigrationNode.m_hasOccupant[i] = false;
						}
					}
				}
			}

			if (veh_tree->appearance_data.applies)
			{
				if (sync_type == rage::SyncType::UPDATE)
				{
					if (auto cmd = sync_src.getCommand())
					{
						if (cmd->last_veh_appearance_update != object_id)
						{
							cmd->last_veh_appearance_update = object_id;
							if (cmd->veh_appearance_ratelimit.canRequest())
							{
								cmd->veh_appearance_ratelimit.addRequest();
							}
							else
							{
								g_hooking.veh_appearance_sync_blamer.setBlame(sync_src);
							}
						}
					}
				}

				if (veh_tree->appearance_data.m_wheelType != 255
					&& veh_tree->appearance_data.m_wheelMod != 0
					)
				{
					const int max_mod = getMaxWheelModForType(veh_tree->appearance_data.m_wheelType);
					if (((veh_tree->appearance_data.m_wheelMod - 1) > max_mod)
						|| (veh_tree->appearance_data.m_hasDifferentRearWheel && ((veh_tree->appearance_data.m_rearWheelMod - 1) > max_mod))
						)
					{
						sync_tally.add(FlowEvent::SE_CRASH, "TF");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							ret = false;
						}
					}
				}
			}

			/*if (veh_tree->m_vehicleGameStateNode.applies)
			{
				if (veh_tree->m_vehicleGameStateNode.m_radioStation > 34)
				{
					sync_tally.add(FlowEvent::SE_INVALID, "TF");
					if (sync_tally.reactions & REACTION_BLOCK)
					{
						ret = false;
					}
				}
			}*/

			if (object_type == NET_OBJ_TYPE_TRAIN)
			{
				g_hooking.train_sync_blamer.setBlame(sync_src);

				auto train_tree = static_cast<CTrainSyncTree*>(tree);

				if (train_tree->train_game_state.applies)
				{
					if (train_tree->train_game_state.m_TrackID >= 12 // Signed, but game can deal with negative numbers, just not overflow.
						|| (train_tree->train_game_state.m_TrackID < 0 && train_tree->train_game_state.m_IsEngine) // Actually, unless it's an engine, then CNetObjTrain::Update's call to CTrain::SetTrackActive will write to arbitrary memory.
						)
					{
						sync_tally.add(FlowEvent::SE_CRASH, "T9");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							train_tree->train_game_state.m_IsEngine = false;
							train_tree->train_game_state.m_TrackID = -1;
						}
					}

					// This can lag the game.
					if (train_tree->train_game_state.m_TrackID != -1)
					{
						if (train_tree->train_game_state.m_TrainConfigIndex == -1
							//|| (train_tree->train_game_state.m_TrackID == 0 && train_tree->train_game_state.m_TrainConfigIndex != 8 && train_tree->train_game_state.m_TrainConfigIndex != 10)
							//|| (train_tree->train_game_state.m_TrackID == 3 && train_tree->train_game_state.m_TrainConfigIndex != 27)
							|| (train_tree->train_game_state.m_TrackID == 9 /* && ??? */)
							|| (train_tree->train_game_state.m_TrackID == 11 /* && ??? */)
							)
						{
							sync_tally.add(FlowEvent::SE_INVALID, "TE");
							if (sync_tally.reactions & REACTION_BLOCK)
							{
								ret = false;
							}
						}
					}

					// Again signed values where the game handles underflow but not overflow.
					if (train_tree->train_game_state.m_TrainConfigIndex >= 28
						|| train_tree->train_game_state.m_CarriageConfigIndex >= 28
						)
					{
						sync_tally.add(FlowEvent::SE_CRASH, "TD");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							train_tree->train_game_state.m_TrainConfigIndex = -1;
							train_tree->train_game_state.m_CarriageConfigIndex = -1;
						}
					}
				}
			}
		}

		const auto pos = tree->getPos(object_type);

		if (tree->attachApplies(object_type))
		{
			auto attached_to = tree->getAttachedTo(object_type);
			if (attached_to != 0)
			{
				// Recursive attachment
				std::unordered_set<uint16_t> attached_obj_ids{};
				attached_obj_ids.emplace(object_id);
				for (auto attid = attached_to; true; )
				{
					if (attached_obj_ids.contains(attid))
					{
						if (attached_obj_ids.size() != 2 || object_type != NET_OBJ_TYPE_PLAYER) // parachute
						{
							sync_tally.add(FlowEvent::SE_CRASH, "T3");
						}
						attached_to = 0;
						tree->setAttachedTo(object_type, 0);
						break;
					}

					attached_obj_ids.emplace(attid);
					if (auto obj = (*pointers::network_object_mgr)->find_object_by_id(attid, true))
					{
						if (auto ent = obj->GetEntity())
						{
							if (auto attattent = ent->GetAttachedTo())
							{
								if (auto attattobj = reinterpret_cast<CDynamicEntity*>(attattent)->m_net_object)
								{
									attid = attattobj->object_id;
									if (attid != 0)
									{
										continue;
									}
								}
							}
						}
					}
					break;
				}
				if (attached_to != 0)
				{
					// Invalid parachute model
					if (object_type == NET_OBJ_TYPE_OBJECT
						&& sync_type == rage::SyncType::CREATE
						&& !reinterpret_cast<CEntitySyncTreeBase*>(tree)->entity_script_info.applies
						&& !AbstractModel(model).isValidParachute()
						&& model != ATSTRINGHASH("hei_prop_heist_magnet")
						&& model != ATSTRINGHASH("prop_container_05a")
						&& model != ATSTRINGHASH("prop_contr_03b_ld")
						)
					{
						sync_tally.add(FlowEvent::SE_CRASH, "T2");
						ret = false;
					}

					bool affects_our_vehicle = g_player_veh.isNetObjId(attached_to);
					hash_t attached_to_model = 0;
					if (auto obj = (*pointers::network_object_mgr)->find_object_by_id(attached_to, true))
					{
						if (auto ent = obj->GetEntity())
						{
							if (ent->archetype)
							{
								attached_to_model = ent->archetype->hash;
							}

							// Bad trailer attachment
							if (object_type == NET_OBJ_TYPE_TRAILER
								&& ent->type != ENTITY_TYPE_VEHICLE
								)
							{
								sync_tally.add(FlowEvent::SE_CRASH, "T1");
								tree->setAttachedTo(object_type, 0);
							}

							if (!affects_our_vehicle
								&& ent->type == ENTITY_TYPE_PED
								)
							{
								//Util::toast(fmt::format("Attachment to a ped (vehicle = {}, attto = {})", (void*)static_cast<CPed*>(ent)->getVehicle(), (void*)ent->GetAttachedTo()), TOAST_ALL);
								if (g_player_veh.isValid()
									&& (g_player_veh.getPointer() == ent->GetAttachedTo() // This tends to update a bit faster than getVehicle
										|| g_player_veh.getPointer() == static_cast<CPed*>(ent)->getVehicle()
										)
									)
								{
									//Util::toast("Attachment to a ped in our vehicle", TOAST_ALL);
									affects_our_vehicle = true;
								}
							}
						}
					}

					// Attachment spam
					if (AbstractModel(model).isSusAttachment()
						|| AbstractModel(attached_to_model).isSusAttachee()
						)
					{
						if (object_type != NET_OBJ_TYPE_PICKUP) // "the air freight buy mission where the shipment is dropped from a titan"
						{
							if (auto c = sync_src.getCommand())
							{
								if (c->sus_attachment_ratelimit.canRequest())
								{
									c->sus_attachment_ratelimit.addRequest();
								}
								else
								{
									sync_tally.add(FlowEvent::SYNCIN_ATTACH);
									if (sync_tally.reactions & REACTION_BLOCK)
									{
										//netSyncTree->setAttachedTo(object_type, 0);
										ret = false;
									}
								}
							}
						}
					}

					if (affects_our_vehicle && g_player_veh.isOwner() // Something attached to the vehicle we're driving?
						&& object_type != NET_OBJ_TYPE_TRAILER // Trailers are obviously okay
						&& (object_type != NET_OBJ_TYPE_PED || model != ATSTRINGHASH("g_m_m_chigoon_02") || !g_player_veh.getModel().usesAttachmentCameras()) // Driving terrorbyte/avenger, other player using turret

						&& !g_player_veh.getModel().isCargobob() // If we're piloting a cargobob, the vehicle we're hooked onto is okay.
						&& !g_player_veh.getModel().isTowTruck() // If we're driving a towtruck, the vehicle we're hooked onto is okay.
						&& (object_type != NET_OBJ_TYPE_OBJECT || (
							model != ATSTRINGHASH("Prop_V_Hook_S") && model != ATSTRINGHASH("hei_prop_heist_magnet") // If we're being hooked onto, the hook being attached to us is okay.
							&& !AbstractModel(model).isValidParachute() // If a ped is parachuting out of our heli, that's okay.
							&& model != ATSTRINGHASH("prop_cs_ciggy_01") // https://github.com/calamity-inc/Stand-Feedback/issues/340
							))

						&& is_session_freeroam() // Missions can have some funny business going on: https://www.guilded.gg/stand/groups/x3ZgB10D/channels/47eb8ea9-21be-4642-8916-b062b4f9cac0/forums/683741132
						&& (object_type != NET_OBJ_TYPE_PLAYER || !g_player_veh.getModel().canBeStoodUpon())

						&& sync_type != rage::SyncType::CONTROL // Apparently happens in some missions. Our logging for GIVE_CONTROL_EVENT + sync tree is abysmal, so I don't know why.
						)
					{
						sync_tally.add(FlowEvent::SE_INVALID, "TA");
						if (sync_tally.reactions & REACTION_BLOCK)
						{
							tree->setAttachedTo(object_type, 0);
						}
					}
				}
			}
		}

		return ret;
	}

	bool on_invalid_model_sync(rage::NetworkObjectType netObjectType, int32_t model)
	{
		floweventreaction_t reactions = sync_src.getReactions(FlowEvent::SYNCIN_IMS);
		toast_t toast_flags = flow_event_reactions_to_toast_flags(reactions);
		if (toast_flags != 0)
		{
			Util::toast(LANG_FMT("PTX_IMS_T", FMT_ARG("player", sync_src.getName()), FMT_ARG("hash", joaatToString(model)), FMT_ARG("type", NetworkObjectType_names[netObjectType])), toast_flags);
		}
		sync_src.applyReactions(reactions);
		return (reactions & REACTION_BLOCK);
	}

	bool check_custom_model_reactions(int32_t model)
	{
		auto entry = g_hooking.custom_model_sync_reactions.find(model);
		if (entry != g_hooking.custom_model_sync_reactions.end())
		{
			floweventreaction_t reactions = entry->second.getReactions(sync_src.getType());
			toast_t toast_flags = flow_event_reactions_to_toast_flags(reactions);
			if (toast_flags != 0)
			{
				Util::toast(LANG_FMT("PTX_CSB_T", FMT_ARG("player", sync_src.getName()), FMT_ARG("hash", joaatToString(model))), toast_flags);
			}
			sync_src.applyReactions(reactions);
			return (reactions & REACTION_BLOCK);
		}
		return false;
	}

	bool is_valid_model(rage::NetworkObjectType type, int32_t model)
	{
		switch (type)
		{
		case NET_OBJ_TYPE_AUTOMOBILE:
			return VEHICLE::IS_THIS_MODEL_A_CAR(model)
				|| VEHICLE::IS_THIS_MODEL_A_QUADBIKE(model)
				|| VEHICLE::IS_THIS_MODEL_AN_AMPHIBIOUS_CAR(model)
				|| VEHICLE::IS_THIS_MODEL_AN_AMPHIBIOUS_QUADBIKE(model)
				;

		case NET_OBJ_TYPE_TRAILER:
			return AbstractModel(model).isTrailer();

		case NET_OBJ_TYPE_BIKE:
			return VEHICLE::IS_THIS_MODEL_A_BIKE(model);

		case NET_OBJ_TYPE_BOAT:
			return VEHICLE::IS_THIS_MODEL_A_BOAT(model);

		case NET_OBJ_TYPE_HELI:
			return is_valid_model_heli(model);

		case NET_OBJ_TYPE_OBJECT:
			return is_valid_model_object(model);

		case NET_OBJ_TYPE_PICKUP:
		case NET_OBJ_TYPE_PICKUP_PLACEMENT:
			return is_valid_model_pickup(model);

		case NET_OBJ_TYPE_PED:
		case NET_OBJ_TYPE_PLAYER:
			return is_valid_model_ped(model);

		case NET_OBJ_TYPE_PLANE:
			return VEHICLE::IS_THIS_MODEL_A_PLANE(model);

		case NET_OBJ_TYPE_SUBMARINE:
			return AbstractModel(model).isSubmarine();

		case NET_OBJ_TYPE_TRAIN:
			return VEHICLE::IS_THIS_MODEL_A_TRAIN(model);
		}
		// NET_OBJ_TYPE_DOOR is ignored
		return true;
	}

	bool is_valid_model_ped(int32_t model)
	{
		if (STREAMING::IS_MODEL_A_PED(model))
		{
			switch (model)
			{
			case ATSTRINGHASH("slod_human"):
			case ATSTRINGHASH("slod_large_quadped"):
			case ATSTRINGHASH("slod_small_quadped"):
				break;

			default:
				return true;
			}
		}
		return false;
	}

	bool is_valid_model_object(int32_t model)
	{
		SOUP_IF_UNLIKELY (!AbstractModel(model).isObject())
		{
			return false;
		}

		// These objects don't crash the game instantly, but they were not made to be spawned in as objects (despite not being world object), so they end up having their rage::fragType unloaded despite still existing in the world. You can imagine how having an object with a stale archtype pointer in the world ends... (CObjectFragmentDrawHandler::AddToDrawList hook attempts to patch this, but there are plenty of failure points...)
		// Some of these seem to crash in CLightEntity code; these depend on having a parent entity which likely gets deleted when switching session.
		SOUP_IF_UNLIKELY (g_objects_crash.contains(model))
		{
			return false;
		}

		return true;
	}

	// Could probably use the same logic as is_valid_model_object, but this should reduce attack surface.
	bool is_valid_model_pickup(int32_t model)
	{
		SOUP_IF_UNLIKELY (!AbstractModel(model).isObjectStrict())
		{
			return false;
		}
		SOUP_IF_UNLIKELY (g_objects_crash.contains(model))
		{
			return false;
		}
		return true;
	}

	bool is_valid_model_heli(int32_t model)
	{
		switch (AbstractModel(model).getVehicleType())
		{
		case VEHICLE_TYPE_HELI:
		case VEHICLE_TYPE_BLIMP:
			return true;
		}
		return false;
	}

	void __fastcall CNetObjVehicle_SetVehicleControlData(uintptr_t _this, CVehicleControlDataNode* data)
	{
		SOUP_IF_UNLIKELY (data->m_isSubCar)
		{
			auto obj = reinterpret_cast<CNetObjVehicle*>(_this - 0x200);
			auto veh = static_cast<CVehicle*>(obj->game_obj);
			SOUP_IF_UNLIKELY (!veh->GetSubHandling())
			{
#if false // Might not always be initialised from give_control_event
				sync_tally.add(FlowEvent::SE_CRASH, "PV");
#else
				sync_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, "PV");
#endif
				data->m_isSubCar = false;
				OG(CNetObjVehicle_SetVehicleControlData)(_this, data);
				data->m_isSubCar = true;
				return;
			}
		}
#ifdef STAND_DEBUG
		else
		{
			auto obj = reinterpret_cast<CNetObjVehicle*>(_this - 0x200);
			auto veh = static_cast<CVehicle*>(obj->game_obj);
			if (veh->GetSubHandling())
			{
				Util::toast("vehicle does have sub handling but was not declared to be a sub car?!");
			}
		}
#endif
		OG(CNetObjVehicle_SetVehicleControlData)(_this, data);
	}

	enum JoinSpecificSessionFlags : unsigned int
	{
		INTEND_TO_JOIN = 1 << 0,
		INTEND_TO_SPECTATE = 1 << 4,

		FROM_INVITE = 1 << 5,
		UNK_FROM_FRIEND = 1 << 1, // Must be removed to join invite only sessions.
		PARTY_BOOT = 1 << 9, //  "Your party is in a GTA Online session. Would you like to join?"
		PARTY_JOIN = 1 << 10, // "Your party has started a Job. Would you like to join?"
		PARTY_JVP = 1 << 11, // "You have entered a party that is in a GTA Online session. Would you like to join?"
		FROM_QUEUE = 1 << 12, // Hides loading spinner when fetching session details and shows "There is a slot available in the GTA Online session you are currently queuing for. Would you like to join?"

		HIDE_SPINNER = 1 << 2, // Seems to hide the loading spinner when fetching session details.
		SKIP_CONFIRMATION_AND_SPECTATE = 1 << 7, // Automatically pulls you up, no "would you like to join a new session?" crap, but also forces you to be a spectator.
		BYPASS_CHECKS = 1 << 14, // Bypasses friend & crew checks.

		UNK_3 = 1 << 3,
		UNK_5 = 1 << 5,
		UNK_6 = 1 << 6,
		UNK_8 = 1 << 8, // Apparently controls the "Are you sure you want to join a GTA Online session?" message, but that's shown anyway.
		UNK_13 = 1 << 13,
	};

	void __fastcall InviteMgr_AcceptInvite(InviteMgr* _this, rage::rlSessionInfo* info, rage::rlGamerInfo* invitee, rage::rlGamerHandle* inviter, unsigned int inviteId, unsigned int inviteFlags, int gameMode, const char* uniqueMatchId, int inviteFrom, int inviteType)
	{
#ifdef STAND_DEBUG
		//g_logger.log(fmt::format("join_specific_session({}, {}, {}, {}, {:x}, flags = {:x}, {:x}, {:x}, {:x}, {:x})", a1, (void*)a2, (void*)a3, (void*)a4, a5, flags, a7, a8, a9, a10));
#endif

		if (g_hooking.join_as_spectator)
		{
			g_hooking.join_as_spectator = false;

			inviteFlags |= INTEND_TO_SPECTATE;
			inviteFlags &= ~INTEND_TO_JOIN;
		}

		if (g_hooking.managed_invite)
		{
			g_hooking.managed_invite = false;

			inviteFlags |= FROM_INVITE;
			inviteFlags &= ~UNK_FROM_FRIEND;
			//inviteFlags |= BYPASS_CHECKS; // causes join to fail as of 2824
		}

		return OG(InviteMgr_AcceptInvite)(_this, info, invitee, inviter, inviteId, inviteFlags, gameMode, uniqueMatchId, inviteFrom, inviteType);
	}

#if false
	bool __fastcall CNetworkSession_JoinSession(CNetworkSession* _this, rage::rlSessionInfo* info, int kSlotType, int nJoinFlags, rage::rlGamerHandle* pReservedGamers, int numReservedGamers)
	{
		// nJoinFlags = 3
		if (g_hooking.managed_join)
		{
			g_hooking.managed_join = false;
			kSlotType = 1;
			//nJoinFlags |= 1; // via invite
			//nJoinFlags |= 2; // set if (nJoinFlags & 1) == 0 && kSlotType == RL_SLOT_PRIVATE
			//nJoinFlags |= 4; // causes join to fail as of 2824
			//nJoinFlags |= 0x40; // joining as a ceo
		}
		return OG(CNetworkSession_JoinSession)(_this, info, kSlotType, nJoinFlags, pReservedGamers, numReservedGamers);
	}
#endif

	// If we're the host, this is also used to serialise info of other players.
	bool __fastcall rage_playerDataMsg_SerMsgPayload_datExportBuffer(rage::datExportBuffer* bb, CNetGamePlayerDataMsg* msg)
	{
		const auto old_nat_type = msg->m_nat_type;
		const auto old_group = msg->m_matchmaking_group;
		const auto old_flags = msg->m_player_flags;

		if (!g_player.isHost() || RageConnector::isSpyActive())
		{
			msg->m_nat_type = g_hooking.ll_lube;

			if (g_hooking.override_mm_group != -1)
			{
				msg->m_matchmaking_group = g_hooking.override_mm_group;
				if (g_hooking.override_mm_group == 4)
				{
					msg->m_player_flags |= CNetGamePlayerDataMsg::PLAYER_MSG_FLAG_SPECTATOR;
				}
				else
				{
					msg->m_player_flags &= ~CNetGamePlayerDataMsg::PLAYER_MSG_FLAG_SPECTATOR;
				}
			}
		}
		else
		{
			if (g_tunables.getInt(TUNAHASH("nat_open_as_host.others"), 0))
			{
				msg->m_nat_type = rage::NET_NAT_OPEN;
			}
		}

		auto ret = OG(rage_playerDataMsg_SerMsgPayload_datExportBuffer)(bb, msg);

		msg->m_nat_type = old_nat_type;
		msg->m_matchmaking_group = old_group;
		msg->m_player_flags = old_flags;

		return ret;
	}

#define PLAYER_BD_PTR(base, size) reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ScriptGlobal(base).at(g_player, size).as<void*>()) - 4)

#define OVERRIDE_GLOBAL_FOR_THIS_PLAYER(global, value) \
beginBadSgTamperCodeThatWillLandUsInHell(); \
auto addr = global.as<int*>(); \
endBadSgTamperCodeThatWillLandUsInHell(); \
to_recover.emplace_back(addr, *addr); \
*addr = value;

	void __fastcall rage_netArrayHandlerBase_WriteUpdate(rage::netArrayHandlerBase* _this, const rage::netPlayer* player, rage::datBitBuffer* bitBuffer, uint16_t updateSeq, unsigned int* currentElement, bool logSizes)
	{
		bool force_recalculate = false;
		std::vector<std::pair<int*, int>> to_recover{};

		SOUP_IF_UNLIKELY (g_hooking.pools_closed_kick.isActive())
		{
			beginBadSgTamperCodeThatWillLandUsInHell();
			ScriptGlobal(GSBD).set<int>(g_hooking.pools_closed_kick.contains(player) ? 5 : 4);
			force_recalculate = (static_cast<rage::netArrayHandler*>(_this)->m_Array == ScriptGlobal(GSBD).as<void*>());
			endBadSgTamperCodeThatWillLandUsInHell();
		}

		if (g_hooking.org_spy)
		{
			beginBadSgTamperCodeThatWillLandUsInHell();
			force_recalculate = (static_cast<rage::netArrayHandler*>(_this)->m_Array == PLAYER_BD_PTR(GLOBAL_PLAYERREMOTE_BASE, GLOBAL_PLAYERREMOTE_SIZE));
			endBadSgTamperCodeThatWillLandUsInHell();
			if (force_recalculate)
			{
				if (auto boss = AbstractPlayer(player->player_id).getBoss(); boss.isValid())
				{
					OVERRIDE_GLOBAL_FOR_THIS_PLAYER(ScriptGlobal(GLOBAL_PLAYERREMOTE_BASE).at(g_player, GLOBAL_PLAYERREMOTE_SIZE).at(GLOBAL_PLAYERREMOTE_ORG).at(GLOBAL_ORG_BOSS), boss);
				}
			}
		}

		SOUP_IF_LIKELY (!force_recalculate)
		{
			OG(rage_netArrayHandlerBase_WriteUpdate)(_this, player, bitBuffer, updateSeq, currentElement, logSizes);
			return;
		}

		// Make sure we recalculate the sync data instead of sending what we already had
		soup::Bytepatch patch;
		uint8_t bytes[] = {
			soup::Bytepatch::JMP1, // jz -> jmp
		};
		patch.initPatch(pointers::rage_netArrayHandlerBase_WriteUpdate_patch, bytes, COUNT(bytes));

		OG(rage_netArrayHandlerBase_WriteUpdate)(_this, player, bitBuffer, updateSeq, currentElement, logSizes);

		for (const auto& e : to_recover)
		{
			*e.first = e.second;
		}
	}

#ifdef STAND_DEBUG
	static std::unordered_set<std::string> triggered_acs{};

	static void onAcTriggered(std::string name, bool only_notify_once = false)
	{
		if (!triggered_acs.contains(name))
		{
			/*if (g_this_dll_is_passive)
			{
				MessageBoxA(nullptr, fmt::format("Anti-Cheat Triggered ({})", name).c_str(), "Stand Passive DLL", MB_OK | MB_ICONEXCLAMATION);
			}
			else*/
			{
				Util::toast(fmt::format("Anti-Cheat Triggered ({})", name), TOAST_ALL);
			}
			if (only_notify_once)
			{
				triggered_acs.emplace(std::move(name));
			}
		}
	}
#endif

	bool __fastcall rage_rlTelemetry_Export(/* rage::RsonWriter */ void* rw, const char* name, uint64_t metricTime, rage::rlMetric* metric)
	{
		bool allow = true;
		__try
		{
			std::string metric_name = metric->getName();
			switch (rage::atStringHash(metric_name))
			{
			case ATSTRINGHASH("CHEAT"): // MetricCHEAT, related to SP cheat box
			case ATSTRINGHASH("AUX_DEUX"): // MetricTamper
			case ATSTRINGHASH("CCF_UPDATE"): // MetricInfoChange
			case ATSTRINGHASH("BENCHMARK_Q"): // was only in 2699 build "TODO: <File description>"
			case ATSTRINGHASH("REPORTER"):
			case ATSTRINGHASH("MEM_NEW"):
			case ATSTRINGHASH("DEBUGGER_ATTACH"):
			case ATSTRINGHASH("XP_LOSS"):
			case ATSTRINGHASH("WEATHER"):
			case ATSTRINGHASH("CASH_CREATED"):
			case ATSTRINGHASH("DR_PS"):
			case ATSTRINGHASH("GARAGE_TAMPER"):
			case ATSTRINGHASH("FAIL_SERV"):
			case ATSTRINGHASH("CODE_CRC"):
			//case ATSTRINGHASH("COLLECTIBLE"): // Whenever we collect a collectible
			case ATSTRINGHASH("DUPE_DETECT"):
			//case ATSTRINGHASH("MISMATCH"): // added in 3095, seems to be related to remote peers doing shit
			case ATSTRINGHASH("MISMATCH_TRIGGERED_EVENT"): // added in 3095
			case ATSTRINGHASH("BLAST"):
#ifdef STAND_DEBUG
				onAcTriggered(metric_name);
#endif
				[[fallthrough]];
			case ATSTRINGHASH("AWARD_XP"): // earning XP
			case ATSTRINGHASH("EARN"): // doing transactions, apparently better to block this for modded money methods
			case ATSTRINGHASH("W_L"): // wanted level
			case ATSTRINGHASH("FLOW_LOW"): // also related to wanted level
			case ATSTRINGHASH("ESVCS"): // emergency service called
			case ATSTRINGHASH("IDLEKICK"): // whenever the game calls PLAYSTATS_IDLE_KICK, which is a lot, not desirable with "Disable Idle Kick" I guess
			case ATSTRINGHASH("REPORT_INVALIDMODEL"): // also used to report remote peers so a bit spammy
			case ATSTRINGHASH("FIRST_VEH"): // also used to report remote peers so a bit spammy
			case ATSTRINGHASH("LAST_VEH"): // also used to report remote peers so a bit spammy
				allow = false;
				break;

			case ATSTRINGHASH("MM"): // loaded modules delta
				{
					std::string data = metric->getLogData();
					if (data.find(soup::ObfusString("=534C5F").str()) != std::string::npos
						|| data.find(soup::ObfusString("|534C5F").str()) != std::string::npos
						)
					{
						allow = false; // a module starts with "SL_", that shouldn't happen
					}
					else
					{
#if false
						// MM metric was submitted, now we can kill this AC thing, because this thing can leak memory and eventually crash the game.
						// Unfortunately, killing this thing will cause a ban within 1-2 days when the user earns ton of money.
						if (auto update_element = pointers::game_skeleton->findUpdateElement(0xA0F39FB6))
						{
							update_element->m_UpdateFunction = reinterpret_cast<rage::gameSkeleton::fnUpdateFunction>(pointers::nullsub);
						}
#endif
					}
				}
				break;
			}

			// This _should_ be fully handled in CommandPlayer, but just in case...
			const bool is_remote_cheat = (soup::ObfusString("DIG") == metric_name);
			if (is_remote_cheat)
			{
				std::string sub_name = static_cast<MetricRemoteCheat*>(metric)->getSubName();
				//metric_name.push_back(':');
				//metric_name.append(sub_name);
				auto gamer_handle = rage::rlGamerHandle::fromString(reinterpret_cast<MetricRemoteCheat*>(metric)->gamer_handle_string);
				if (gamer_handle.isValid())
				{
					auto p = AbstractPlayer::fromRockstarId(gamer_handle.rockstar_id);
					if (auto cngp = p.getCNetGamePlayer())
					{
						int id = -1;
						switch (rage::atStringHash(sub_name))
						{
						case ATSTRINGHASH("SCRIPT"): // MetricCheatScript
							id = 64;
							break;

						case ATSTRINGHASH("CF"): // MetricCheatCRCRequestFlood
							id = 0;
							break;

						case ATSTRINGHASH("CC"): // MetricCheatCRCCompromised
							id = 1;
							break;

						case ATSTRINGHASH("CNR"): // MetricCheatCRCNotReplied
							id = 2;
							break;

						case ATSTRINGHASH("CCF"): // MetricRemoteInfoChange
							id = 4;
							break;

						case ATSTRINGHASH("CCT"): // MetricRemoteInfoChange2
							id = 5;
							break;

						case ATSTRINGHASH("RTB"): // MetricRemoteInfoChange3
							id = 6;
							break;

						case ATSTRINGHASH("GSCW"): // MetricRemoteInfoChange4
							id = 7;
							break;

						case ATSTRINGHASH("GSCB"): // MetricRemoteInfoChange5
							id = 8;
							break;

						case ATSTRINGHASH("GSINV"): // MetricRemoteInfoChange6
							id = 9;
							break;

						case ATSTRINGHASH("GSINT"): // MetricRemoteInfoChange7
							id = 10;
							break;
						}

						if (id == -1 || id >= 16 || !((cngp->m_cheatsNotified >> id) & 1))
						{
							if (id != 64) // This is only triggered by bad sport leave stuff, which isn't really what we're interested in.
							{
								std::string str = fmt::to_string(id);
								str.push_back('*');
								p.triggerDetection(FlowEvent::MOD_RAC, std::move(str), 50);
							}
						}
					}
				}
			}

#ifdef STAND_DEBUG
			if (g_hooking.log_metrics
				&& metric_name != "WALKMODE"
				)
			{
				/*__try
				{
					std::string rtti_name = soup::rtti::object::fromInstance(metric)->getTypeInfo()->getName();
					name.append(" (");
					name.append(rtti_name);
					name.push_back(')');
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
				}*/
				if (!allow)
				{
					metric_name.append(" [blocked]");
				}
				metric_name.append(": ");
				metric_name.push_back('{');
				metric_name.append(metric->getLogData());
				metric_name.push_back('}');
				g_logger.log(std::move(metric_name));
				//GENERATE_STACK_TRACE;
			}
#endif
		}
		__EXCEPTIONAL()
		{
		}
		SOUP_IF_LIKELY (allow)
		{
			__try
			{
				return OG(rage_rlTelemetry_Export)(rw, name, metricTime, metric);
			}
			__EXCEPTIONAL()
			{
			}
		}
		return false;
	}

#if HTTP_HOOK
	char __fastcall rage_netHttpRequest_Update(rage::netHttpRequest* a1)
	{
#ifdef STAND_DEBUG
		if (g_hooking.log_http_requests)
		{
			__try
			{
				auto msg = fmt::format("{}://{}{}", a1->Scheme, a1->Host, a1->Path);
				if constexpr (false)
				{
					if (a1->m_QueuedChunks.m_tail && a1->m_QueuedChunks.m_tail->m_Data.m_Buf)
					{
						msg.append(" - ").append((const char*)a1->m_QueuedChunks.m_tail->m_Data.m_Buf);
					}
				}
				static std::unordered_set<std::string> dedup{};
				if (!dedup.contains(msg)
					|| false
					)
				{
					dedup.emplace(msg);
					g_logger.log(std::move(msg));
				}
			}
			__EXCEPTIONAL()
			{
			}
		}
#endif
		std::string path(a1->Path);
		bool block = false;
#if BLOCK_ALL_METRICS
		if (path.find(soup::ObfusString("Telemetry.asmx/Submit").str()) != std::string::npos)
		{
			block = true;
		}
		else
#endif
			if (path.find(soup::ObfusString("GameTransactions.asmx").str()) != std::string::npos)
		{
			__try
			{
				const bool is_bonus = (path.find(soup::ObfusString("GameTransactions.asmx/Bonus").str()) != std::string::npos);
				if (a1->m_QueuedChunks.m_tail && a1->m_QueuedChunks.m_tail->m_Data.m_Buf)
				{
					auto inst = soup::json::decode((const char*)a1->m_QueuedChunks.m_tail->m_Data.m_Buf);
					if (auto items = inst->asObj().find(soup::ObfusString("items").str()))
					{
						for (const auto& item : items->asArr())
						{
							const auto itemId = item.asObj().at(soup::ObfusString("itemId").str()).asInt().value;
							switch (itemId)
							{
							default:
								if (!is_bonus)
								{
									break;
								}
								[[fallthrough]];
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_SE"): // -1862553257
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_NO_FALL"): // 1604862494
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_NOT_SEEN"): // -1454779202
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_NO_DEATH"): // -2114390367
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_EV_1"): // -1807935122
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_EV_2"): // 2026037878
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_EV_3"): // 1794983659
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_EV_4"): // 612415999
							case ATSTRINGHASH("SERVICE_EARN_JBONUS_MODEL"): // -782113305
#ifdef STAND_DEBUG
								onAcTriggered(fmt::to_string(itemId));
#endif
								[[fallthrough]];
							case 1354049168:
								block = true;
								break;
							}
						}
					}
				}
				else if (is_bonus)
				{
					block = true;
#ifdef STAND_DEBUG
					onAcTriggered("Bonus");
#endif
				}
			}
			__EXCEPTIONAL()
			{
			}
		}
		if (block)
		{
			if (g_hooking.redir_host.empty())
			{
				g_hooking.redir_host = soup::ObfusString("hcaptcha.com").str();
			}
			a1->Host = g_hooking.redir_host.c_str();
		}
		return OG(rage_netHttpRequest_Update)(a1);
	}
#endif

	void __fastcall rage_netEventMgr_SendEvent(rage::netEventMgr* a1, rage::netGameEvent* evt)
	{
		__try
		{
#ifdef STAND_DEBUG
			if (g_hooking.log_net_event_send)
			{
				std::vector<CNetGamePlayer*> targets{};
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					if (CNetGamePlayer* ngp = p.getCNetGamePlayer(); evt->IsInScope(ngp))
					{
						targets.emplace_back(ngp);
					}
				}

				std::string msg = "Sending ";
				msg.append(get_net_event_name(evt->type).getEnglishUtf8());
				msg.append(" to ");
				if (targets.size() == 1)
				{
					msg.append(targets.at(0)->GetLogName());
				}
				else
				{
					msg.append(std::to_string(targets.size()));
					msg.append(" players");
				}
				Util::toast(std::move(msg));
			}
#endif
		}
		__EXCEPTIONAL()
		{
		}

		bool allow = true;
		switch (evt->type)
		{
		case NETWORK_CHECK_CODE_CRCS_EVENT:
#ifdef STAND_DEBUG
			onAcTriggered("CCCE", true);
#endif
			allow = false;
			break;

		case REPORT_MYSELF_EVENT:
#ifdef STAND_DEBUG
			onAcTriggered("RME", true);
#endif
			allow = false;
			break;

		case REPORT_CASH_SPAWN_EVENT:
#ifdef STAND_DEBUG
			onAcTriggered("CSE", true);
#endif
			allow = false;
			break;
		}

		auto& bitset = evt->event_id->player_flags;

		switch (evt->type)
		{
			case NETWORK_PLAY_SOUND_EVENT:
				if (g_hooking.play_sound_whitelist.contains(evt))
				{
					bitset = g_hooking.play_sound_whitelist.consumeForBitset(evt);
				}
				break;

			case NETWORK_STOP_SOUND_EVENT:
				if (g_hooking.stop_sound_whitelist.contains(evt))
				{
					bitset = g_hooking.stop_sound_whitelist.consumeForBitset(evt);
				}
				break;
		}

#if defined(STAND_DEBUG) && false
		if (allow
			&& evt->type == NETWORK_PLAY_SOUND_EVENT
			)
		{
			uint8_t n = 0;
			for (compactplayer_t p = 0; p != MAX_PLAYERS; ++p)
			{
				if (evt->event_id->player_flags & (1 << p))
				{
					++n;
				}
			}

			const auto data = (CPlaySoundEvent*)evt;
			Util::toast(fmt::format("Sent a sound event to {} player(s). ({}, {}, {})", n, data->m_setNameHash));
		}
#endif

		if (allow)
		{
			OG(rage_netEventMgr_SendEvent)(a1, evt);
		}
		else
		{
			evt->FlagForRemoval();
		}
	}

#if CLEAR_BONUS_ON_DL
	void __fastcall CTunables_OnCloudEvent(uintptr_t a1, CloudEvent* evt)
	{
		std::string buf;
		__try
		{
			auto tunables = reinterpret_cast<CTunables*>(a1 - 8);
			if (evt->type == 0) // EVENT_REQUEST_FINISHED
			{
				if (evt->data.request_id == tunables->cloud_request_id
					&& evt->data.success
					&& evt->data.data
					&& *pointers::rage_AES_ms_cloudAes
					)
				{
					if (pointers::rage_AES_Decrypt(
						*pointers::rage_AES_ms_cloudAes,
						evt->data.data,
						evt->data.size
					))
					{
						auto jt = soup::json::decodeForDedicatedVariable(evt->data.data);

						jt->asObj().at(soup::ObfusString("bonus").str()).asArr().children.clear();
						jt->asObj().at(soup::ObfusString("tunables")).asObj().erase(soup::ObfusString("8B7D3320").str());

						buf = jt->encode();

						evt->data.data = buf.data();
						evt->data.size = (unsigned int)buf.size();

						pointers::rage_AES_Encrypt(
							*pointers::rage_AES_ms_cloudAes,
							evt->data.data,
							evt->data.size
						);
					}
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		__try
		{
			return OG(CTunables_OnCloudEvent)(a1, evt);
		}
		__EXCEPTIONAL()
		{
		}
		buf.clear();
	}
#endif

	// ARXAN CHECKED
	void rage_gameSkeleton_updateGroup_Update(void* __this)
	{
		auto* const _this = reinterpret_cast<rage::gameSkeleton::updateGroup*>(__this);
		for (rage::gameSkeleton::updateBase* element = _this->m_Head; element != nullptr; element = element->m_Next)
		{
			SOUP_IF_UNLIKELY (element->m_Name == 0xA0F39FB6)
			{
				static_cast<rage::gameSkeleton::updateElement*>(element)->m_UpdateFunction = reinterpret_cast<rage::gameSkeleton::fnUpdateFunction>(pointers::nullsub);
			}
			element->Update();
		}
	}

	// CNetwork::Bail, ARXAN CHECKED
	// Something about just hooking this function causes the game to not bail when it normally should (because BE is disabled).
	// When Stand is injected late, however, we do need to be more active and block the bail.
	void __fastcall network_bail(void* bailParams, bool bSendScriptEvent)
	{
		__try
		{
			if (*reinterpret_cast<int*>(bailParams) != BAIL__NO_BATTLEYE__MEGAMIND_EMOJI)
			{
#ifdef STAND_DEBUG
				Util::toast(fmt::format("CNetwork::Bail, reason = {}", *reinterpret_cast<int*>(bailParams)), TOAST_ALL);
#endif
				OG(network_bail)(bailParams, bSendScriptEvent);
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	[[nodiscard]] static bool is_address_in_game_region(void* addr) noexcept
	{
		return g_gta_module.range.contains(addr);
	}

	[[nodiscard]] static bool is_unwanted_dependency(rage::sysDependency* dep) noexcept
	{
		void* f1 = dep->m_Callback;
		void* f2 = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(dep) + 0x100);
		void* f3 = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(dep) + 0x1A0);

		if (!is_address_in_game_region(f1) || !is_address_in_game_region(f2) || !is_address_in_game_region(f3))
		{
			return false;
		}

		if (*reinterpret_cast<uint8_t*>(f1) != 0xE9)
		{
			return false;
		}

		return true;
	}

	struct AcVerifier
	{
		virtual ~AcVerifier() = 0;
		virtual bool run() = 0;

		/* 0x08 */ rage::sysObfuscated_Mutate<uint32_t> m_last_time;
		/* 0x18 */ rage::sysObfuscated_Mutate<uint32_t> m_delay;
	};
	static_assert(sizeof(AcVerifier) == 0x28);

	static bool nulldep(const rage::sysDependency&)
	{
		return true;
	}

	// Adapted from https://github.com/YimMenu/YimMenu/blob/master/src/hooks/misc/queue_dependency.cpp
	void __fastcall rage_sysDependencyScheduler_InsertInternal(rage::sysDependency* dep)
	{
		__try
		{
			SOUP_IF_UNLIKELY (is_unwanted_dependency(dep))
			{
#ifdef STAND_DEBUG
				//Util::toast("Blocking AC verifier", TOAST_ALL);
#endif
				auto verifier = reinterpret_cast<AcVerifier*>(reinterpret_cast<uintptr_t>(dep) - 0x30);
				verifier->m_delay.Set(INT_MAX);
				dep->m_Callback = &nulldep;
				*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(dep) + 0x100) = &nulldep;
				*reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(dep) + 0x1A0) = &nulldep;
				return;
			}
		}
		__EXCEPTIONAL()
		{
		}
		__try
		{
			OG(rage_sysDependencyScheduler_InsertInternal)(dep);
		}
		__EXCEPTIONAL()
		{
		}
	}

	bool network_can_access_multiplayer(void* a1, int* error)
	{
		if (error)
		{
			*error = 0;
		}
		return true;
	}

	bool __fastcall parse_presence_event(CGamePresenceEventDispatcher* a1, rage::rlGamerInfo* a2, __int64 a3, const char** data, const char* source)
	{
		// observed values for source: "self", "inbox"
		__try
		{
#ifdef STAND_DEBUG
			if (g_hooking.log_presence_event_game)
			{
				g_logger.log(fmt::format("Game Presence Event: {}, {}, {}", source, a2->toString(), *data));
			}
#endif
			rapidjson::Document d;
			if (d.Parse(*data).HasParseError() || !d.IsObject())
			{
#ifdef STAND_DEBUG
				Util::toast(fmt::format("Blocked presence event with bad json: {}", *data), TOAST_ALL);
#endif
				return false;
			}
			auto gm_evt = d[soup::ObfusString("gm.evt").c_str()].GetObj();
			auto event_type = gm_evt[soup::ObfusString("e").c_str()].GetString();
			switch (rage::atStringHash(event_type))
			{
			case ATSTRINGHASH("ginv"):
				{
					if (g_hooking.block_job_invites)
					{
						return false;
					}

					auto event_data = gm_evt[soup::ObfusString("d").c_str()].GetObj();
					auto flags = event_data[soup::ObfusString("f").c_str()].GetUint64();
					if (flags >= 0x5000000000000000)
					{
//#ifdef STAND_DEBUG
//						Util::toast("blocked invite with bad flags", TOAST_ALL);
//#endif
						return false;
					}
				}
				break;

				// Letter Scraps: {"gm.evt":{"e":"StatUpdate","d":{"stat":-2044299740,"from":"MKII_Griefer","ival":50}}}
			case ATSTRINGHASH("StatUpdate"):
				if (g_hooking.block_friend_stat_notifications)
				{
					return false;
				}
				break;
			}
		}
		__EXCEPTIONAL()
		{
		}
		return OG(parse_presence_event)(a1, a2, a3, data, source);
	}

	bool __fastcall rgsc_RgscPresenceManager_OnSocialClubEvent(void* a1, const rage::rlScPresenceMessage& msg, void* a3)
	{
		__try
		{
#ifdef STAND_DEBUG
			if (g_hooking.log_presence_event_scdll)
			{
				g_logger.log(fmt::format("SC DLL Presence Event: {}", msg.m_Contents));
				// "ros.gamer.online":"SC 64157314"
				// "ros.gamer.textmessage":{"h":"SC 111509622","n":"quake_1337"}
				// "ros.publish":{"channel":"self","msg":{"gm.evt":{"e":"ginv","d":{"v":5,"s":"RqNCxhGraxOAwdl3h0AAAAAAFcnCDU+AgncWbFgRgCjtwf6rEW0WCGOfOH2csS04u6j2Do7n9/9glltpU2X3AR9pyq2PkjolVKiMP2bk/c4+Wf36uS6SL9Z+3wBebh0/////wAA/////wAAQGvBopEatrE4a8GilRq2sTg==","h":"OrzLDgAAAAADAAAAAAAAAA==","n":"_llisenok_","gm":0,"c":"F1aMI4_RjkqGufccGa-48A","cc":"AAAAAAAAAAAAAAAAAAAAAA==","mid":"","if":0,"it":0,"l":0,"p":0,"f":32808,"ed":3901228386,"gd":1516504418,"u":1434550707,"cr":53096025}}}}
			}
#endif
			const char* i = msg.m_Contents;
			if (auto jr = soup::json::decode(i))
			{
				std::string name = std::move(jr->asStr().value);
				if (rage::atStringHash(name) == ATSTRINGHASH("ros.gamer.textmessage"))
				{
					++i; // skip ':'
					jr = soup::json::decode(i);
					if (jr)
					{
						const int64_t sender_rid = rage::rlGamerHandle::fromString(jr->asObj().at("h").asStr().value.c_str()).rockstar_id;
						std::string sender_name = std::move(jr->asObj().at("n").asStr().value);
						std::string contents = msg.m_Contents;

						auto b = HttpRequestBuilder{ HttpRequestBuilder::POST, "scui.rockstargames.com", "/api/messaging/getmessages" }
						.addHeader("Authorization", ScAccount::getAuthorizationHeaderValue())
						.addHeader("X-Requested-With", "XMLHttpRequest") // needed to avoid "CSRF" error
						.addHeader("Content-Type", "application/json");
						b.setPayload(std::move(std::string(R"({"env":"prod","title":"gta5","version":11,"senderRockstarId":")").append(fmt::to_string(sender_rid)).append(R"(","pageIndex":0})")));
						b.setResponseCallback([sender_rid, sender_name{ std::move(sender_name) }, contents{ std::move(contents) }](soup::HttpResponse&& resp)
						{
							bool should_block = false;
							std::string msg_contents{};
							if (auto jr = soup::json::decode(resp.body); jr && jr->isObj())
							{
								if (auto MessageList = jr->asObj().find("MessageList"); MessageList && MessageList->isObj())
								{
									if (auto Messages = MessageList->asObj().find("Messages"); Messages && Messages->isArr() && !Messages->asArr().children.empty())
									{
										if (BgScript::hasFunction("dm"))
										{
											msg_contents = Messages->asArr().children.at(0)->asObj().at("Text").asStr().value;
											should_block = BgScript::query("dm", sender_rid, msg_contents, /* since 114.2 */ sender_name);
										}
									}
								}
							}
							if (!should_block)
							{
								// This is not an ad; trigger notifications now.
								if (g_hooking.scdm_scnotify || msg_contents.empty())
								{
									rage::rlScPresenceMessage presmsg{ 0, contents.c_str() };
									OG(rgsc_RgscPresenceManager_OnSocialClubEvent)(nullptr, presmsg, nullptr);
								}
								if (!msg_contents.empty())
								{
									if (auto tf = g_hooking.scdm_toasts.getToastFlags())
									{
										Util::toast(LANG_FMT("SCDMT_T", FMT_ARG("sender", sender_name), FMT_ARG("text", msg_contents)), tf);
									}
								}
							}
						})
						.dispatch();
						return false; // Swallow this event. If this is not an ad, we will show the SC notification later.
					}
				}
			}
			return OG(rgsc_RgscPresenceManager_OnSocialClubEvent)(a1, msg, a3);
		}
		__EXCEPTIONAL()
		{
		}
		return false;
	}

	// When a rope is being deleted, inform vehicle gadgets that store ropeInstance pointers.
	// This prevents various nasty things when spawning a towtruck and then using "Delete All Ropes".
	void __fastcall rage_ropeManager_Remove(rage::ropeManager* _this, rage::ropeInstance* pInst)
	{
		__try
		{
			AbstractEntity::getAllVehicles([pInst](AbstractEntity&& ent)
			{
				auto veh = ent.getCVehicle();
				for (const auto& gadget : veh->m_pVehicleGadgets)
				{
					switch (gadget->GetType())
					{
					case VGT_PICK_UP_ROPE:
					case VGT_PICK_UP_ROPE_MAGNET:
						static_cast<CVehicleGadgetPickUpRope*>(gadget)->processRopeInstanceBeingRemoved(pInst);
						break;

					case VGT_TOW_TRUCK_ARM:
						static_cast<CVehicleGadgetTowArm*>(gadget)->processRopeInstanceBeingRemoved(pInst);
						break;
					}
				}
				CONSUMER_CONTINUE;
			});
		}
		__EXCEPTIONAL()
		{
		}
		OG(rage_ropeManager_Remove)(_this, pInst);
	}

	// rage::rlSession::OnSessionEvent in newer rage versions
	char __fastcall CNetworkSession_OnSessionEvent(CNetworkSession* thisptr, __int64 a2, rage::snEvent* event)
	{
		__try
		{
			//Util::toast(fmt::format("SessionEvent {} {}", (uint32_t)event->GetId(), soup::rtti::object::fromInstance(event)->getTypeInfo()->getName()), TOAST_ALL);
			switch (event->GetId())
			{
			case SESSION_EVENT_ADDED_GAMER:
				//Util::toast(fmt::format("snEventAddedGamer {}", reinterpret_cast<rage::snEventAddedGamer*>(event)->gamer.name), TOAST_ALL);
				LeaveReasons::forget(reinterpret_cast<rage::snEventAddedGamer*>(event)->gamer);
				if (!reinterpret_cast<rage::snEventAddedGamer*>(event)->gamer.isUser())
				{
					auto ts = ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>();
					if (is_session_transition_active(ts, false))
					{
						if (ts == TRANSITION_STATE_WAIT_JOIN_FM_SESSION)
						{
							CommandBlockBlockJoin::session_players.emplace_back(SessionPlayer(reinterpret_cast<rage::snEventAddedGamer*>(event)->gamer));
						}
					}
					else
					{
#if HAVE_BREAKUP_KICK
						if (process_join(&reinterpret_cast<rage::snEventAddedGamer*>(event)->gamer, false))
						{
							auto rid = reinterpret_cast<rage::snEventAddedGamer*>(event)->gamer.getHandle().rockstar_id;
							FiberPool::queueJob([rid]
							{
								if (auto host = AbstractPlayer::getHost(); host != g_player)
								{
									host.remoteDesync(rage::rlGamerHandle(rid));
								}
							});
							return 0;
						}
#endif
					}
				}
				break;

			case SESSION_EVENT_REMOVED_GAMER:
				if (!reinterpret_cast<rage::snEventRemovedGamer*>(event)->gamer.isUser())
				{
					LeaveReasons::onRemovedGamer(
						reinterpret_cast<rage::snEventRemovedGamer*>(event)->gamer,
						reinterpret_cast<rage::snEventRemovedGamer*>(event)->remove_reason
					);
					if (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>() == TRANSITION_STATE_WAIT_JOIN_FM_SESSION)
					{
						for (auto i = CommandBlockBlockJoin::session_players.begin(); i != CommandBlockBlockJoin::session_players.end(); ++i)
						{
							if (i->rid == reinterpret_cast<rage::snEventRemovedGamer*>(event)->gamer.getHandle().rockstar_id)
							{
								CommandBlockBlockJoin::session_players.erase(i);
								break;
							}
						}
					}
				}
				break;
			}
			return OG(CNetworkSession_OnSessionEvent)(thisptr, a2, event);
		}
		__EXCEPTIONAL()
		{
		}
		return 0;
	}

	bool __fastcall CNetworkSession_OnHandleJoinRequest(CNetworkSession* _this, rage::snSession* pSession, rage::rlGamerInfo* gamerInfo, void* snJoinRequest, const unsigned int sessionType)
	{
		soup::Bytepatch patch;
		__try
		{
			if (process_join(gamerInfo, true))
			{
				patch.initPatchNOP(pointers::custombjmsg_nop2bytes, 2);
				*pointers::custombjmsg_responsecode = g_hooking.host_bj_message;
			}
		}
		__EXCEPTIONAL()
		{
		}
		bool ret = OG(CNetworkSession_OnHandleJoinRequest)(_this, pSession, gamerInfo, snJoinRequest, sessionType);
		*pointers::custombjmsg_responsecode = 0x18;
		return ret;
	}

	unsigned int __fastcall rage_netIceSession_GetAdditionalLocalCandidates(void* _this, rage::netSocketAddress* addrs, unsigned int maxAddrs)
	{
		if (g_hooking.force_relay_connections)
		{
			return 0;
		}
		return OG(rage_netIceSession_GetAdditionalLocalCandidates)(_this, addrs, maxAddrs);
	}

#define EXTRACT_VISUAL_SETTINGS false

	static void processVisualConfigOverride(float& res, const std::string& name)
	{
#if EXTRACT_VISUAL_SETTINGS
		g_logger.log(fmt::format(R"(VIS: "{}", {})", name, res));
#endif
		if (auto e = g_hooking.visual_config_overrides.find(name); e != g_hooking.visual_config_overrides.end())
		{
			res = e->second;
		}
	}

	float __fastcall get_visual_config_float(void* config, const char* prop, float fallback)
	{
		float res = OG(get_visual_config_float)(config, prop, fallback);
		__try
		{
			processVisualConfigOverride(res, prop);
		}
		__EXCEPTIONAL()
		{
		}
		return res;
	}

	[[nodiscard]] static std::string combineWithDot(std::string l, std::string r)
	{
		l.push_back('.');
		l.append(r);
		return l;
	}

	float __fastcall get_visual_config_float_with_directory(void* config, const char* dir, const char* prop, float fallback)
	{
		float res = OG(get_visual_config_float_with_directory)(config, dir, prop, fallback);
		__try
		{
			processVisualConfigOverride(res, combineWithDot(dir, prop));
		}
		__EXCEPTIONAL()
		{
		}
		return res;
	}

	float* __fastcall get_visual_config_colour(void* config, float* a2, const char* prop)
	{
		float* res = OG(get_visual_config_colour)(config, a2, prop);
		__try
		{
			processVisualConfigOverride(res[0], combineWithDot(prop, "red"));
			processVisualConfigOverride(res[1], combineWithDot(prop, "green"));
			processVisualConfigOverride(res[2], combineWithDot(prop, "blue"));
		}
		__EXCEPTIONAL()
		{
		}
		return res;
	}

	camBaseObject* __fastcall camFactory_CreateObject(const camBaseObjectMetadata* metadata, const rage::ClassId* classIdToVerify)
	{
		if (g_hooking.disable_cam_shake)
		{
			switch (classIdToVerify->name_hash)
			{
			case ATSTRINGHASH("camBaseFrameShaker"):
				return nullptr;
			}
		}
		return OG(camFactory_CreateObject)(metadata, classIdToVerify);
	}

	// void __fastcall CNetworkObjectMgr::UpdateAllObjectsForRemotePlayers(CNetworkObjectMgr* this, const rage::netPlayer* sourcePlayer)
	static char __fastcall some_player_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* player)
	{
		sync_src = player->player_id;

		if (!((sync_src.getReactions(FlowEvent::SYNCIN_CLONE_CREATE, false) | sync_src.getReactions(FlowEvent::SYNCIN_CLONE_UPDATE, false) | sync_src.getReactions(FlowEvent::SYNCIN_CLONE_DELETE, false)) & REACTION_BLOCK))
		{
			__try
			{
				return g_hooking.some_player_sync_hook.getOriginal<decltype(&some_player_sync)>()(mgr, player);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				sync_src.getAndApplyReactionsIn(FlowEvent::SE_CRASH, "E1");
			}
		}
		return 0;
	}

	unsigned int __fastcall rage_aiTaskTree_UpdateTask(rage::aiTaskTree* tree, rage::aiTask* task, float timeStep)
	{
		__try
		{
			if (task->type_id == CTaskTypes::TASK_TAKE_OFF_PED_VARIATION)
			{
				if (!AbstractModel(static_cast<CTaskTakeOffPedVariation*>(task)->object_model).isScubaObject())
				{
					switch (static_cast<CTaskTakeOffPedVariation*>(task)->object_model)
					{
					case ATSTRINGHASH("p_parachute_s"):
					case ATSTRINGHASH("reh_p_para_bag_reh_s_01a"):
					case ATSTRINGHASH("p_para_bag_tr_s_01a"):
					case ATSTRINGHASH("p_para_bag_xmas_s"):
					case ATSTRINGHASH("lts_p_para_bag_lts_s"):
					case ATSTRINGHASH("lts_p_para_bag_pilot2_s"):
					case ATSTRINGHASH("vw_p_para_bag_vine_s"):
						break;

					default:
						if (g_gui.doesRootStateAllowCrashPatches())
						{
							Util::onPreventedCrash("A1", Util::to_padded_hex_string(static_cast<CTaskTakeOffPedVariation*>(task)->object_model));
							return 0;
						}
					}
				}
			}
			else if (task->type_id == CTaskTypes::TASK_FALL)
			{
				if (g_hooking.graceful_landing)
				{
					//Util::toast(fmt::format("CTaskFall: {:X}", static_cast<CTaskFall*>(task)->flags));

					/* Flags:
					* 0x00C00 - able to turn (FF_DisableNMFall + FF_HasAirControl)
					* 0x01000 - FF_DampZVelocity
					* 0x10000 - beast landing (FF_BeastFall)
					*/
					static_cast<CTaskFall*>(task)->flags |= 0x11C00;
				}
			}
			else if (task->type_id == CTaskTypes::TASK_IN_VEHICLE_SEAT_SHUFFLE)
			{
				// Prevent us being frozen when we're in a 2-seater like the ruffian. See scripts/shuffle-freeze.pluto.
				if (g_player_veh == static_cast<CTaskInVehicleSeatShuffle*>(task)->m_pVehicle
					&& static_cast<CTaskInVehicleSeatShuffle*>(task)->m_iTargetSeatIndex == -1
					&& g_player_veh.getDriver() == g_player_ped
					)
				{
					Util::toast(LOC("INVTASK"));
					task->state = 10; // fast-track to finish state
				}
			}
			else if (task->type_id == CTaskTypes::TASK_VEHICLE_GOTO_PLANE
				|| task->type_id == CTaskTypes::TASK_VEHICLE_LAND_PLANE // This one also has problematic code in ProcessPreFSM
				|| task->type_id == CTaskTypes::TASK_VEHICLE_FLY_DIRECTION
				)
			{
				if (task->entity->type != ENTITY_TYPE_VEHICLE
					|| static_cast<CVehicle*>(task->entity)->vehicle_type != VEHICLE_TYPE_PLANE
					)
				{
					if (g_gui.doesRootStateAllowCrashPatches())
					{
						Util::onPreventedCrash("A2", std::to_string(task->type_id));
						return 0;
					}
				}
			}
			else if (task->type_id == CTaskTypes::TASK_VEHICLE_GOTO_HELICOPTER
				|| task->type_id == CTaskTypes::TASK_VEHICLE_LAND
				)
			{
				if (task->entity->type != ENTITY_TYPE_VEHICLE
					|| !static_cast<CVehicle*>(task->entity)->InheritsFromHeli()
					)
				{
					if (g_gui.doesRootStateAllowCrashPatches())
					{
						Util::onPreventedCrash("A2", std::to_string(task->type_id));
						return 0;
					}
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		__try
		{
			return OG(rage_aiTaskTree_UpdateTask)(tree, task, timeStep);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash("A0", std::to_string(task->type_id));
		}
		return 0;
	}

	void __fastcall CControl_StartPlayerPadShake(void* _this, unsigned int lightDuration, int lightIntensity, unsigned int heavyDuration, int heavyIntensity, int DelayAfterThisOne)
	{
		__try
		{
			evtPadShakeEvent::trigger(evtPadShakeEvent(lightDuration, lightIntensity, heavyDuration, heavyIntensity, DelayAfterThisOne));
		}
		__EXCEPTIONAL()
		{
		}
		OG(CControl_StartPlayerPadShake)(_this, lightDuration, lightIntensity, heavyDuration, heavyIntensity, DelayAfterThisOne);
	}

	void __fastcall rage_FriendsReaderTask_Complete(rage::FriendsReaderTask* task, rage::netTaskStatus status)
	{
		OG(rage_FriendsReaderTask_Complete)(task, status);

		if (status == rage::NET_TASKSTATUS_SUCCEEDED)
		{
			std::unordered_set<int64_t> real_friends;

			for (uint32_t i = 0; i != *task->num_friends; ++i)
			{
				real_friends.emplace(task->friends[i].sc_friend.rockstar_id);
			}

			for (const auto& g : RemoteGamer::tracked)
			{
				if (g->fake_friend
					&& !real_friends.contains(g->rid)
					)
				{
					if (auto name = ScAccount::fromRID(g->rid)->getNameLazy())
					{
						task->addFriend(name, g->rid, g->isOnline(), g->hasSession());
					}
				}
			}

			std::sort(&task->friends[0], &task->friends[*task->num_friends], [](const rage::rlFriend& a, const rage::rlFriend& b)
			{
				if (a.IsInSession() != b.IsInSession())
				{
					return a.IsInSession();
				}
				if (a.sc_friend.IsOnline() != b.sc_friend.IsOnline())
				{
					return a.sc_friend.IsOnline();
				}
				return false;
			});
		}
	}

	CVehicleModelInfo* __fastcall CModelInfo_AddVehicleModel(const char* name, bool permanent, unsigned int mapTypeDefIndex)
	{
		// permanent = false
		// mapTypeDefIndex = 0xF000

		if (!CustomDlcMgr::isLoadingADlc())
		{
			return OG(CModelInfo_AddVehicleModel)(name, permanent, mapTypeDefIndex);
		}

		//Util::toast(fmt::format("Registering custom model {}", name), TOAST_ALL);
		auto info = (CustomVehicleModel*)malloc(sizeof(CustomVehicleModel) + strlen(name));
		pointers::CVehicleModelInfo_ctor(info);
		info->Init();
		info->hash = rage::atStringHash(name);
		strcpy(info->hash_name, name);
		pointers::rage_fwArchetypeManager_RegisterStreamedArchetype(info, mapTypeDefIndex);
		CustomDlcMgr::registerModel(info);
		return info;
	}

	void __fastcall CNetObjVehicle_GetVehicleCreateData(uintptr_t _this, CVehicleCreationDataNode* data)
	{
		OG(CNetObjVehicle_GetVehicleCreateData)(_this, data);

		if (g_hooking.spoof_custom_models)
		{
			if (auto info = CustomDlcMgr::getCustomModelInfo(data->model))
			{
				if (const auto spoof = CommandDlcpacks::getSpoofForVehicleType(info->vehicle_type); spoof.has_value())
				{
					data->model = spoof.value();
				}
			}
		}
	}

#if STAND_DEBUG
	void rage_netMessage_WriteHeader(unsigned int id, rage::datBitBuffer& buf)
	{
		__try
		{
			if (g_hooking.log_packet_write)
			{
				g_logger.log(fmt::format("Writing packet: {}", rage::netMessage_getName(id)));
			}
		}
		__EXCEPTIONAL()
		{
		}
		return OG(rage_netMessage_WriteHeader)(id, buf);
	}
#endif
}
