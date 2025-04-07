#pragma once

#include <Windows.h>

#include <soup/Bytepatch.hpp>

#include "dx_common.hpp"
#include "function_types.hpp"
#include "gta_fwddecl.hpp"

#include "conf_antianticheat.hpp"
#include "conf_netcode.hpp"

namespace Stand
{
	struct pointers
	{
		static inline HWND hwnd{};

		static inline int32_t* CLoadingScreens_ms_Context{};
		static inline void* game_logos{};
		static inline void* game_license{};

		static inline uint32_t* native_scramble_count{};

		static inline bool* is_session_started{};
		static inline const char* online_version{};
		static inline const char* build_version{};

		static inline CPedFactory** ped_factory{};
		static inline CNetworkPlayerMgr** network_player_mgr{};
		static inline rage::netEventMgr** CNetwork_sm_EventMgr{};

		static inline rage::scrNativeRegistrationTable* native_registration_table{};

		static inline rage::atArray<GtaThread*>* script_threads{};
		static inline rage::scrProgram** script_programs{};
		static inline uint64_t script_programs_size{};
		static inline int* rage_scrProgram_sm_GlobalSizes{};
		static inline rage::scrValue** rage_scrProgram_sm_Globals{};

		static inline CGameScriptHandlerMgr** script_handler_mgr{};

		static inline uint8_t* model_spawn_bypass{};

		static inline CReplayInterfaceGame** replay_interface{};
		static inline CTheScripts_GetGUIDFromEntity_t CTheScripts_GetGUIDFromEntity{};
		static inline uint32_t* extension_id_script_guid{};
		static inline rage::fwPool<rage::fwScriptGuid>** script_guid_pool{};

		static inline rage::fwPool<camBaseCamera>** cam_pool{};

		static inline CHandlingDataMgr* handling_data_mgr{};

		static inline create_and_send_pickup_rewards_event_t create_and_send_pickup_rewards_event{};
		static inline create_and_send_increment_stat_event_t create_and_send_increment_stat_event{};

		static inline rage_rlClan_GetPrimaryMembership_t rage_rlClan_GetPrimaryMembership{};
		static inline rage::rlClanClient* clan_client{};

		static inline CExplosionEvent_Trigger_t CExplosionEvent_Trigger{};

		static inline rage::rlGamerInfo* rlPresence_m_ActingGamerInfo{};
		static inline rage::rlGamerInfo* rlPresence_GamerPresences_0_GamerInfo{};

		static inline CTextFile* thetext{};

		static inline CNetworkTextChat** text_chat{};
		static inline CMultiplayerChat** chat_box{};
		static inline wchar_t* rage_ioKeyboard_sm_TextBuffer{};
		static inline uint32_t* min_character_value{};
		static inline CMultiplayerChat_OnEvent_t CMultiplayerChat_OnEvent{};
		static inline uint32_t* sm_systemTime_m_Time{};
		static inline CMultiplayerChat_SetFocus_t CMultiplayerChat_SetFocus{};
		static inline bool* rage_ioKeyboard_sm_AllowLocalKeyboardLayout{};
		static inline rage::fwProfanityFilter* profanity_filter{};
		static inline rage_fwProfanityFilter_AsteriskProfanity_t rage_fwProfanityFilter_AsteriskProfanity{};
		static inline CNewHud_UpdateImeText_t CNewHud_UpdateImeText{};

		static inline send_net_event_ack_t send_net_event_ack{};

		static inline read_bitbuffer_dword_t read_bitbuffer_dword{};
		static inline rage_datBitBuffer_ReadBits_t rage_datBitBuffer_ReadBits{};
		static inline rage_datBitBuffer_WriteBits_t rage_datBitBuffer_WriteBits{};

		static inline rage_datBitBuffer_WriteUnsigned_t rage_datBitBuffer_WriteUnsigned{};

		static inline rage_datImport_netPeerAddress_t rage_datImport_netPeerAddress{};

		static inline uint16_t* is_explosion_type_valid_patch{};

		static inline uint8_t* clamp_gameplay_cam_pitch{};

		static inline hash_t* lock_radio_lsur_hash{};
		static inline hash_t* radio_talk02_hash{};
		static inline soup::Bytepatch self_radio_patch{};

		static inline void* err_net_array{};

		static inline void* script_vm_switch{};
		static constexpr auto script_vm_num_opcodes = 131;
		static inline void* script_vm_jump_table[131] = { nullptr };
		static inline void* script_thread_error_kill_1{};
		static inline void* script_thread_error_kill_2{};

		static inline IDXGISwapChain** swapchain{};
		static inline void* swapchain_resize_buffers{};
		static inline void* swapchain_present{};

		inline static rage::fwRenderThreadInterface* render_thread_interface{};
		inline static CViewportManager* viewport_manager{};

		static inline CVehicle_SetGravityForWheellIntegrator_t CVehicle_SetGravityForWheellIntegrator{};

		static inline uint64_t* menu_ptr{};
		static inline CPlayerListMenu_HandleContextOption_t CPlayerListMenu_HandleContextOption{};
		static inline uint8_t* CPlayerListMenu_handler_jnz{};
		static inline uint8_t* CPlayerListMenu_handler_jz{};
		static inline rage::rlGamerHandle* rlGamerHandle_buffer{};

		static inline send_session_info_request_t send_session_info_request{};
		//static inline EventDispatcher* rl_presence_event_dispatcher{};
		//static inline dispatch_event_t dispatch_event{};
		static inline InviteMgr* invite_mgr{};
		static inline InviteMgr_HandleJoinRequest_t InviteMgr_HandleJoinRequest{};

		static inline rage_rlSessionManager_QueryDetail_t rage_rlSessionManager_QueryDetail{};

		static inline rage_netPeerAddress_Export_t rage_netPeerAddress_Export{};

		static inline find_object_by_id_t find_object_by_id{};
		static inline rage_fwArchetype_GetFragType_t rage_fwArchetype_GetFragType{};

		static inline CNetworkSession_KickPlayer_t CNetworkSession_KickPlayer{};

		static inline WaterQuad** water_quads{};
		static inline uint16_t* water_quads_size{};
		static inline int32_t* world_x_min{};
		static inline int32_t* world_x_max{};
		static inline int32_t* world_y_min{};
		static inline int32_t* world_y_max{};

		static inline uint32_t* entity_extension_id_head_blend_data{};

		static inline uint8_t* get_bink_movie_time_pre_ret{};

		static inline soup::Bytepatch watchdog_thread_crash_patch{};

		static inline soup::Bytepatch point_gun_at_head_patch{};

		static inline void* cleanup_sync_trees{};
		static inline void* net_object_mgr_cleanup{};

		static inline uint8_t* pseudo_object_check{};
		static inline soup::Bytepatch model_blacklist_patch{};

		static inline bool* rage__audRequestedSettings__sm_IgnoreBlockedAudioThread{};

		static inline uint8_t* hide_cursor_jnz{};

		static inline rage::rlRosCredentials* ros_credentials{};

		static inline uint8_t* request_control_spectating_check{};

		static inline uint8_t* sp_ground_snow_check{};

		static inline uint8_t* skybox_patch_ptr{};

		static inline rage::rlPc* g_rlPc{};
		static inline uint32_t* num_presence_attributes{};
		static inline rage::rlScPresenceAttribute* presence_attributes{};

		static inline rgsc_Script_CallFunction_t rgsc_Script_CallFunction{};

		static inline rage::netConnectionManager** connection_mgr{};

		static inline CNetworkSession** network_session{};

		static inline send_MsgTextMessage_t send_MsgTextMessage{}; // sms
		static inline send_netComplaintMsg_t send_netComplaintMsg{};

		static inline rage::rlRosGeoLocInfo* rlRos_s_GeoLocInfo{};

		static inline rage::netPeerId* local_peer_id{};
		static inline CNetworkVoice** voice_chat_data{};

		//static inline rage_rlScMatchmaking_Find_t rage_rlScMatchmaking_Find{};

		static inline CNetworkObjectMgr** network_object_mgr{};

		static inline hash_t* particle_fx_asset{};

		inline static UI3DDrawManager** ui3d_draw_mgr{};

		static inline uint8_t* stop_sound_dont_network{};
		static inline uint8_t* ShouldPerformInVehicleHairScale_jz{};

		static inline uint8_t* UpdateTypers_call_IsInSpectatorMode{};

		static inline bool* weaponwheel_update_isinvehicle_true{};

		static inline bool* hide_hud_and_radar_this_frame{};

		static inline rage_snSession_HandleRemoveGamerCmd_t rage_snSession_HandleRemoveGamerCmd{};

		static inline CBlacklistedGamers_BlacklistGamer_t CBlacklistedGamers_BlacklistGamer{};

		static inline register_file_t register_file{};

		static inline rage_fwEntity_GetAttachedTo_t rage_fwEntity_GetAttachedTo{};

		static inline CMiniMapBlip** blips{};

		static inline CVisualSettings_LoadAll_t CVisualSettings_LoadAll{};

		static inline CVehicleMetadataMgr* vehicle_metadata{};

		static inline void_func_no_args_t add_keyboard_hook{};
		static inline void_func_no_args_t remove_keyboard_hook{};

		static inline rage_rlScPresence_GetAttributesForGamer_t rage_rlScPresence_GetAttributesForGamer{};

		static inline CRagdollRequestEvent_Trigger_t CRagdollRequestEvent_Trigger{};
		static inline CAlterWantedLevelEvent_Trigger_t CAlterWantedLevelEvent_Trigger{};
		static inline CWeaponDamageEvent_Trigger_t CWeaponDamageEvent_Trigger{};

		static inline CRotaryWingAircraft_BreakOffTailBoom_t CRotaryWingAircraft_BreakOffTailBoom{};
		static inline CVehicleDamage_BreakOffWheel_t CVehicleDamage_BreakOffWheel{};
		static inline CPlaySoundEvent_TriggerPos_t CPlaySoundEvent_TriggerPos{}; // -1 cannot be passed to this. Find your own.
		static inline CPlaySoundEvent_TriggerEnt_t CPlaySoundEvent_TriggerEnt{}; // See above.
		static inline CGiveControlEvent_Trigger_t CGiveControlEvent_Trigger{};
		static inline CProjectile_TriggerExplosion_t CProjectile_TriggerExplosion{};

		static inline uint8_t* CPedIntelligence_GetCanCombatRoll_patch{};
		static inline uint8_t* CTaskMotionInVehicle_CheckForThroughWindow_patch{};

		static inline CAudioBankRequestEvent_Trigger_t CAudioBankRequestEvent_Trigger{};

		static inline CGpsSlot* gps_slots{};

		static inline CAircraftDamageBase_BreakOffSection_t CAircraftDamageBase_BreakOffSection{};
		static inline CNetworkPtFXEvent_Trigger_t CNetworkPtFXEvent_Trigger{};
		static inline CStopSoundEvent_Trigger_t CStopSoundEvent_Trigger{};

		static inline float* balanced_cannons_time_between_shots{};
		static inline float* balanced_cannons_alternate_wait_time{};

#if false
		static inline CFireEvent_Trigger_t CFireEvent_Trigger{};
#endif

		static inline rage::atFixedArray<CProjectile*, 50>* projectile_pool{};

#if BLOCK_ALL_METRICS
		static inline uint32_t* s_Policies_m_SubmissionIntervalSeconds{};
#endif

#if CLEAR_BONUS_ON_DL
		static inline void** rage_AES_ms_cloudAes{};
		inline static aes_func_t rage_AES_Decrypt{};
		inline static aes_func_t rage_AES_Encrypt{};
#endif

#if PREVENT_PGSTREAMER_CRASH
		inline static rage::sysObfuscated_Mutate<int>* rage_pgStreamer_smTamperCrash{};
		inline static soup::Bytepatch pgstreamer_crash_patch{};
#endif

#if PREVENT_GBUFFER_CRASH
		inline static uint32_t* GBuffer_m_Attached{};
		inline static soup::Bytepatch gbuffer_crash_patch{};
#endif

#if PREVENT_GBUFFER_CRASH
		inline static uint32_t* rage_grcDevice_sm_KillSwitch{};
		inline static soup::Bytepatch grcdevice_crash_patch{};
#endif

		inline static uint8_t* custombjmsg_nop2bytes{};
		inline static int32_t* custombjmsg_responsecode{};

		static inline CPauseMenu_SetMenuPreference_t CPauseMenu_SetMenuPreference{};
		static inline CProfileSettings** profile_settings{}; // pointer to CProfileSettings::sm_Instance
		static inline CProfileSettings_Set_t CProfileSettings_Set{};
		static inline rage_rlSetLanguage_t rage_rlSetLanguage{};

		static inline CPauseMenu_UpdateProfileFromMenuOptions_t CPauseMenu_UpdateProfileFromMenuOptions{};

		inline static uint8_t* rage_netArrayHandlerBase_WriteUpdate_patch{};

		//inline static uint64_t* session_id{};

		inline static rage::netTimeSync** network_clock{};

		static inline rage_netConnectionManager_SendConnectionless_t rage_netConnectionManager_SendConnectionless{};

#if I_CAN_SPY
		inline static rage_netConnection_QueueFrame_t rage_netConnection_QueueFrame{};
		inline static rage_netConnectionManager_CreateConnection_t rage_netConnectionManager_CreateConnection{};
		inline static CNetworkSession_BuildJoinRequest_t CNetworkSession_BuildJoinRequest{};
		inline static rage_netConnectionManager_DestroyConnection_t rage_netConnectionManager_DestroyConnection{};
		inline static rage_netRelay_GetMyRelayAddress_t rage_netRelay_GetMyRelayAddress{};
		inline static rage_snMsgJoinRequest_Export_t rage_snMsgJoinRequest_Export{};
		inline static rage_snMsgAddGamerToSessionBase_SerMsgPayload_datImportBuffer_t rage_snMsgAddGamerToSessionBase_SerMsgPayload_datImportBuffer{};
#if SPY_CAN_KICK
		inline static rage_netConnectionManager_OpenTunnel_t rage_netConnectionManager_OpenTunnel{};
#endif
#endif
		inline static rage_netConnectionManager_Send_t rage_netConnectionManager_Send{};

		inline static CExtraContentManager** extra_content_manager{};
		inline static CNetworkAssetVerifier** asset_verifier{};

		inline static CVehicleModelInfo_ctor_t CVehicleModelInfo_ctor{};
		inline static rage_fwArchetypeManager_RegisterStreamedArchetype_t rage_fwArchetypeManager_RegisterStreamedArchetype{};

		inline static CExtraContentManager_AddContentFolder_t CExtraContentManager_AddContentFolder{};
		inline static CExtraContentManager_LoadContent_t CExtraContentManager_LoadContent{};

		static inline uint8_t* CPed_WantsToUseActionMode{};

		inline static remove_known_ref_t remove_known_ref{};

		inline static PopMultiplierArea* pop_multiplier_areas{};

		inline static CNetworkAssetVerifier_RefreshEnvironmentCRC_t CNetworkAssetVerifier_RefreshEnvironmentCRC{};

		inline static AcThingData** ac_thing_data{};
#if false
		inline static rage::gameSkeleton* game_skeleton{};
#endif

		inline static uint8_t* FormatInt64ToCash_mov_bAddCurrencyDeliminator{};
		inline static CTextConversion_FormatIntForHumans_t CTextConversion_FormatIntForHumans{};

		inline static uint32_t* authoritative_time_scale_offset{};
		inline static float* rage_fwTimer_sm_fTimeScale{};
		inline static float(*rage_fwTimer_sm_aTimeScales)[4]{};

		inline static uint8_t* netIceSession_terminationTimer_IsTimedOut{};

		inline static rage_netIceSessionOffer_ctor_t rage_netIceSessionOffer_ctor{};
		inline static rage_netIceSessionOffer_SerMsgPayload_datImportBuffer_t rage_netIceSessionOffer_SerMsgPayload_datImportBuffer{};
		inline static rage_netIceSessionAnswer_SerMsgPayload_datImportBuffer_t rage_netIceSessionAnswer_SerMsgPayload_datImportBuffer{};

		inline static CGameWorld_Remove_t CGameWorld_Remove{};
		inline static CPedFactory_DestroyPed_t CPedFactory_DestroyPed{};
		inline static vehicle_commands__DeleteScriptVehicle_t vehicle_commands__DeleteScriptVehicle{};
		inline static CObjectPopulation_DestroyObject_t CObjectPopulation_DestroyObject{};

		inline static rage::netArrayManager** net_array_mgr{};

		inline static rage::fwBasePool** rage_netGameEvent_ms_pPool{};

		inline static rage_rlProfileStats_ReadStatsByGamer_t rage_rlProfileStats_ReadStatsByGamer{};

		inline static rage::atArray<ScaleformMovieStruct>* scaleform_array{};
		inline static CScaleformStore* scaleform_store{};
		inline static GFxValue_ObjectInterface_GetMember_t GFxValue_ObjectInterface_GetMember{};

		inline static void* nullsub{};
	};
}
