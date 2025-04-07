#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include <Windows.h>

#include <soup/ShortString.hpp>

#include "fwddecl.hpp"
#include "gta_fwddecl.hpp"
#include "natives_decl.hpp"

#include "conf_antianticheat.hpp"

#include "Blamer.hpp"
#include "rlclancommon.hpp"
#include "dx_common.hpp"
#include "DetourHook.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "GamerIdentifier.hpp"
#include "gta_entity.hpp"
#include "AssociatedWhitelist.hpp"
#include "netIpAddress.hpp"
#include "NetworkObjectTypes.hpp"
#include "rlGamerId.hpp"
#include "rlSessionInfo.hpp"
#include "rl_matching.hpp"
#include "NetworkedSound.hpp"
#include "Spinlock.hpp"
#include "ToastConfig.hpp"
#include "Whitelist.hpp"

#define HAVE_SUPERJUMP_ANTI_DETECTION false
#define FORCE_SEND_MESSAGES_BY_DEFAULT true

#define OG(name) ::Stand::g_hooking.name##_hook.getOriginal<decltype(&::Stand::hooks::name)>()

namespace Stand
{
	namespace hooks
	{
		unsigned int __fastcall rage_fwConfigManager_GetSizeOfPool(__int64 fwConfigManager, unsigned int pool_hash, unsigned int fallback_value);

		bool __fastcall rage_scrThread_UpdateAll(uint32_t insnCount);

		__int64 swapchain_resize_buffers(__int64 ret); // HRESULT(*)(IDXGISwapChain* chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
		IDXGISwapChain* swapchain_present(); // HRESULT(*)(IDXGISwapChain* chain, UINT sync_interval, UINT flags)
		__int64 __fastcall swapchain_present_streamproof(IDXGISwapChain* thisptr, __int64 a2, __int64 a3, __int64 a4, int cy, __int64 uFlags, __int64 a7);

		LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		[[nodiscard]] bool process_join(rage::rlGamerInfo* player, bool as_host);
		bool send_net_info_to_lobby_wrap(rage::datBitBuffer* a1, rage::rlGamerInfo* gamer_info);
		rage::rlGamerInfo* __fastcall CNetGamePlayer_GetGamerInfo(CNetGamePlayer* player);

		rage::rlClanMembershipData* __fastcall get_active_clan_data(int userIndex);

		float __fastcall get_player_card_stat(CNetObjPlayer* a1, void* a2, unsigned int statIndex, unsigned int a4);

		__int64 __fastcall CPlayerGamerDataNode_Serialise(CPlayerGamerDataNode* node, rage::CSyncDataBase* io_interface);

		void __fastcall data_node_write(rage::netSyncDataNode* node, rage::netSyncTreeTargetObject* obj, rage::datBitBuffer& buf);
		void __fastcall write_player_game_state_data_node(CPlayerGameStateDataNode* node);
		void __fastcall CNetObjVehicle_GetVehicleMigrationData(uintptr_t _this, CVehicleProximityMigrationDataNode* data);

		const char* CTextFile_Get(CTextFile* a1, const char* label);
		const char* CTextFile_GetInternal(CTextFile* a1, uint32_t label_hash);

		void __fastcall received_chat_message(__int64 a1, __int64 a2, const rage::rlGamerHandle* sender, const char* message, bool is_team_chat);
		bool process_chat_message(const rage::rlGamerHandle* sender, const char* message, bool is_team_chat);
		bool __fastcall scaleform_is_key_pressed_1(__int64 a1, int a2, int a3);
		bool __fastcall is_chat_character_valid(wchar_t a1);
		bool __fastcall send_CMsgTextMessage(rage::netConnectionManager* mgr, int32_t con_id, rage::CMsgTextMessage* msg, uint32_t a4, uint64_t a5);
		bool __fastcall CNetworkTextChat_SubmitText(void* _this, rage::rlGamerInfo* gamerInfo, const char* text, bool bTeamChat);
		bool __fastcall CMultiplayerChat_UpdateInputWhenTyping(CMultiplayerChat* _this);

		bool apply_received_script_event_reactions(AbstractPlayer sender_ap, EventAggregation&& e, int64_t* args, int32_t args_count, std::unordered_map<int32_t, std::string>&& index_names);
		[[nodiscard]] bool handle_scripted_game_event(CNetGamePlayer* sender, int64_t* args, int32_t args_count);

		void __fastcall rage_netEventMgr_ProcessEventData(rage::netEventMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, netGameEventId_t event_id, int event_index, int event_handled_bitset, __int64 bitbuffer_size, rage::datBitBuffer* bitbuffer);

		void __fastcall CEventGroupScriptNetwork_DispatchEvent(CEventGroupScriptNetwork* thisptr, CEventNetwork* event);

		bool __fastcall rage_netPlayerMgrBase_SendBuffer(rage::netPlayerMgrBase* a1, const rage::netPlayer* recipient, const void* data, unsigned int size, unsigned int sendFlags, rage::netSequence* seq, const rage::netPlayer* sender);
		CBaseModelInfo* __fastcall rage_fwArchetypeManager_GetArchetypeFromHashKey(Hash modelHash, unsigned int* index);
		long onScriptException(_EXCEPTION_POINTERS* exp) noexcept;
		uint32_t __fastcall script_vm(void** stack, int64_t** script_globals, rage::scrProgram* program, rage::scrThreadContext* thread_ctx);
		[[nodiscard]] std::pair<int, int*> getLastScriptEvent(hash_t script);
		[[nodiscard]] std::string dumpScriptEvent(std::pair<int, int*>& event);
		bool blameScriptCrash(rage::scrThread* thread, std::string& message, bool include_event_dump);
		void logScriptScrash(rage::scrThread* thread, std::string&& message);

		void __fastcall camFrame_copy(camFrame* a1, camFrame* a2);

		bool __fastcall CPhysical_TestNoCollision(CPhysical* _this, rage::phInst* pOtherInst);

		CProjectile* CProjectileManager_CreateProjectile(uint32_t uProjectileNameHash, uint32_t uWeaponFiredFromHash, CEntity* pOwner, const void* mat, float fDamage, int damageType, int effectGroup, const CEntity* pTarget, const void* pNetIdentifier, uint32_t tintIndex, bool bCreatedFromScript, bool bProjectileCreatedFromGrenadeThrow);

		rage::rlGamerHandle* __fastcall rlGamerHandle_from_friend_index(int32_t friend_index);
		bool rage_rlGetGamerStateTask_ParseResults(rage::rlGetGamerStateTask* task);

		//bool __fastcall rage_rlScMatchmaking_Find(int userIndex, unsigned int min_available_slots, const rage::rlMatchingFilter& filter, unsigned int maxResults, rage::rlSessionInfo* outResults, unsigned int* outNumResults, rage::netStatus* outStatus);
		//bool __fastcall rage_rlScMatchmakingFindTask_ReadSession(rage::rlScMatchmakingFindTask* task, rage::parTreeNode* node, rage::rlSessionInfo* outSession);
		//bool __fastcall rage_rlScMatchmakingFindTask_ProcessSuccess(rage::rlScMatchmakingFindTask* task, rage::rlRosResult* a2, rage::parTreeNode* node, int* a4);
		bool __fastcall rage_rlScMatchmaking_Advertise(int localGamerIndex, uint32_t numSlots, uint32_t availableSlots, const rage::rlMatchingAttributes* attrs, uint64_t sessionId, const rage::rlSessionInfo* sessionInfo, rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status);
		bool __fastcall rage_rlScMatchmaking_Unadvertise(int localGamerIndex, const rage::rlScMatchmakingMatchId* matchId, rage::netStatus* status);
		bool __fastcall rage_netTransactor_SendResponse_rage_rlSessionDetailResponse(void* /* rage::netTransactor */ _this, const void* /* rage::netTransactionInfo */ txInfo, rage::rlSessionDetailResponse* msg);

		int __fastcall get_matchmaking_pool();
		uint32_t CExtraContentManager_GetCRC(CExtraContentManager* _this, unsigned int initValue);

		void received_clone_create_ack(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, uint16_t object_id, uint32_t ack_code);
		void received_clone_sync_ack(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, void* buffer, uint16_t object_id, uint32_t ack_code);
		void received_clone_remove_ack(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, uint16_t object_id, uint32_t ack_code);
		void __fastcall received_clone_create(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, rage::NetworkObjectType object_type, uint16_t object_id, uint32_t sync_flags, rage::datBitBuffer* buffer, uint32_t timestamp);
		__int64 __fastcall received_clone_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* recipient, rage::NetworkObjectType object_type, uint16_t object_id, rage::datBitBuffer* buffer, uint32_t unk, uint32_t timestamp);
		void __fastcall clone_pack(CNetworkObjectMgr* mgr, rage::netObject* netObject, CNetGamePlayer* recipient, rage::datBitBuffer* buffer);
		//void __fastcall send_clone_create(rage::netObjectMgrBase* mgr, rage::netObject* obj, CNetGamePlayer* player, rage::datBitBuffer* buffer);
		void __fastcall send_clone_sync(rage::netObjectMgrBase* mgr, CNetGamePlayer* player, rage::netObject* obj, __int64 a4, int16_t* a5, char a6);
		//bool __fastcall send_clone_remove(CNetworkObjectMgr* mgr, rage::netObject* obj, CNetGamePlayer* player, char a4);
		void rage_netSyncTree_Read(rage::netSyncTree* tree, rage::SyncType sync_type, uint32_t sync_flags, rage::datBitBuffer* buffer, rage::netLoggingInterface* logger);
#ifdef STAND_DEV
		bool __fastcall rage_netSyncDataNode_Read(rage::netSyncDataNode* _this, unsigned int serMode, unsigned int actFlags, rage::datBitBuffer* bitBuffer, rage::netLoggingInterface* pLog);
#endif
		bool sync_can_apply(rage::netSyncTree* tree, rage::netObject* netObject);

		[[nodiscard]] bool process_tree_for_tally(rage::netSyncTree* tree, rage::SyncType sync_type, rage::NetworkObjectType object_type, rage::ObjectId object_id, CEntity* ent);
		[[nodiscard]] bool on_invalid_model_sync(rage::NetworkObjectType netObjectType, int32_t model);
		[[nodiscard]] bool check_custom_model_reactions(int32_t model);
		[[nodiscard]] bool is_valid_model(rage::NetworkObjectType type, int32_t model);
		[[nodiscard]] bool is_valid_model_ped(int32_t model);
		[[nodiscard]] bool is_valid_model_object(int32_t model);
		[[nodiscard]] bool is_valid_model_pickup(int32_t model);
		[[nodiscard]] bool is_valid_model_heli(int32_t model);

		void __fastcall CNetObjVehicle_SetVehicleControlData(uintptr_t _this, CVehicleControlDataNode* data);

		void __fastcall InviteMgr_AcceptInvite(InviteMgr* _this, rage::rlSessionInfo* info, rage::rlGamerInfo* invitee, rage::rlGamerHandle* inviter, unsigned int inviteId, unsigned int inviteFlags, int gameMode, const char* uniqueMatchId, int inviteFrom, int inviteType);
		//bool __fastcall CNetworkSession_JoinSession(CNetworkSession* _this, rage::rlSessionInfo* info, int kSlotType, int nJoinFlags, rage::rlGamerHandle* pReservedGamers, int numReservedGamers);
		bool __fastcall rage_playerDataMsg_SerMsgPayload_datExportBuffer(rage::datExportBuffer* bb, CNetGamePlayerDataMsg* msg);

		void __fastcall rage_netArrayHandlerBase_WriteUpdate(rage::netArrayHandlerBase* _this, const rage::netPlayer* player, rage::datBitBuffer* bitBuffer, uint16_t updateSeq, unsigned int* currentElement, bool logSizes);

		bool __fastcall rage_rlTelemetry_Export(/* rage::RsonWriter */ void* rw, const char* name, uint64_t metricTime, rage::rlMetric* metric);
#if HTTP_HOOK
		char __fastcall rage_netHttpRequest_Update(rage::netHttpRequest* a1);
#endif
		void __fastcall rage_netEventMgr_SendEvent(rage::netEventMgr* a1, rage::netGameEvent* evt);
#if CLEAR_BONUS_ON_DL
		void __fastcall CTunables_OnCloudEvent(uintptr_t a1, CloudEvent* evt);
#endif
		void rage_gameSkeleton_updateGroup_Update(void* __this);
		void __fastcall network_bail(void* bailParams, bool bSendScriptEvent);
		void __fastcall rage_sysDependencyScheduler_InsertInternal(rage::sysDependency* dep);
		bool network_can_access_multiplayer(void* a1, int* error);

		bool __fastcall parse_presence_event(CGamePresenceEventDispatcher* a1, rage::rlGamerInfo* a2, __int64 a3, const char** data, const char* source);
		bool __fastcall rgsc_RgscPresenceManager_OnSocialClubEvent(void* a1, const rage::rlScPresenceMessage& msg, void* a3);

		void __fastcall rage_ropeManager_Remove(rage::ropeManager* _this, rage::ropeInstance* pInst);

		char __fastcall CNetworkSession_OnSessionEvent(CNetworkSession* thisptr, __int64 a2, rage::snEvent* event);

		void __fastcall write_player_game_state_data_node(uintptr_t a1, CPlayerGameStateDataNode* node);

		bool __fastcall CNetworkSession_OnHandleJoinRequest(CNetworkSession* _this, rage::snSession* pSession, rage::rlGamerInfo* gamerInfo, void* snJoinRequest, const unsigned int sessionType);

		unsigned int __fastcall rage_netIceSession_GetAdditionalLocalCandidates(void* _this, rage::netSocketAddress* addrs, unsigned int maxAddrs);

		float __fastcall get_visual_config_float(void* config, const char* prop, float fallback);
		float __fastcall get_visual_config_float_with_directory(void* config, const char* dir, const char* prop, float fallback);
		float* __fastcall get_visual_config_colour(void* config, float* a2, const char* prop);
		camBaseObject* __fastcall camFactory_CreateObject(const camBaseObjectMetadata* metadata, const rage::ClassId* classIdToVerify);

		char __fastcall some_player_sync(CNetworkObjectMgr* mgr, CNetGamePlayer* player);
		unsigned int __fastcall rage_aiTaskTree_UpdateTask(rage::aiTaskTree* tree, rage::aiTask* task, float timeStep);

		void __fastcall CControl_StartPlayerPadShake(void* _this, unsigned int lightDuration, int lightIntensity, unsigned int heavyDuration, int heavyIntensity, int DelayAfterThisOne);
		void __fastcall rage_FriendsReaderTask_Complete(rage::FriendsReaderTask* task, rage::netTaskStatus status);

		bool __fastcall rage_AES_isTransformITKeyId(unsigned int keyId);
		void __fastcall rage_AES_TransformITDecrypt(unsigned int selector, void* data, unsigned int size);
		bool __fastcall rage_AES_Decrypt(unsigned int keyId, unsigned int selector, void* data, unsigned int size);
		CVehicleModelInfo* __fastcall CModelInfo_AddVehicleModel(const char* name, bool permanent, unsigned int mapTypeDefIndex);
		void __fastcall CNetObjVehicle_GetVehicleCreateData(uintptr_t _this, CVehicleCreationDataNode* data);

#ifdef STAND_DEBUG
		void rage_netMessage_WriteHeader(unsigned int id, rage::datBitBuffer& buf);
#endif
	}

	class Hooking
	{
	public:
		FlowEventReactionData flow_event_reactions[FlowEvent::SIZE];
		floweventreaction_t player_net_event_reactions[MAX_PLAYERS] = { 0 };
		floweventreaction_t player_sync_in_reactions[MAX_PLAYERS] = { 0 };
		floweventreaction_t player_sync_out_reactions[MAX_PLAYERS + 1] = { 0 };
		bool block_outgoing_syncs_to_host = false;
		time_t player_timeouts[MAX_PLAYERS]{};
		void putInTimeout(compactplayer_t p);

		DetourHook rage_fwConfigManager_GetSizeOfPool_hook;
		float pools_size_multiplier = 1.0f;

		WNDPROC og_wndproc = nullptr;

		bool streamproof_present_works = false;
		DetourHook swapchain_resize_buffers_hook;
		DetourHook swapchain_present_hook;
		DetourHook swapchain_present_streamproof_hook;

		DetourHook rage_scrThread_UpdateAll_hook;
		bool hide_from_player_list = false;
		std::optional<int> og_cayo_flags{};

		DetourHook send_net_info_to_lobby_wrap_hook;
		DetourHook CNetGamePlayer_GetGamerInfo_hook;

		bool spoof_name = false;
		soup::ShortString<16> spoofed_name;
		std::unordered_map<uint64_t, soup::ShortString<16>> name_overrides{};
		[[nodiscard]] static bool isSpoofedNameAllowed(std::string name) noexcept;

		[[nodiscard]] static int64_t getRealRid() noexcept;
		[[nodiscard]] static int64_t getCurrentRid() noexcept;

		DetourHook get_active_clan_data_hook;
		bool spoof_clan_membership = false;
		rage::rlClanMembershipData spoofed_clan_membership{};
		static void forceUpdateGamerDataNode() noexcept;

		DetourHook get_player_card_stat_hook;
		bool spoof_rank = false;
		float spoofed_rank;
		bool spoof_kd = false;
		float spoofed_kd;

		DetourHook CPlayerGamerDataNode_Serialise_hook;
		bool spoof_qa_flag = false;
		bool spoof_cheater_flag = false;

		DetourHook data_node_write_hook;
		// CPlayerGameStateDataNode
		bool block_outgoing_godmode_flag = true;
		bool have_legit_invincible = false;
		bool have_legit_bullet_proof = false;
		bool have_legit_flame_proof = false;
		bool have_legit_explosion_proof = false;
		bool have_legit_collision_proof = false;
		bool have_legit_melee_proof = false;
		bool have_legit_steam_proof = false;
		bool have_legit_smoke_proof = false;
		bool block_outgoing_spectating_flag = true;
		bool block_outgoing_damage_mod = true;
		bool block_outgoing_modded_health = true; // Also CPedHealthDataNode
#if HAVE_SUPERJUMP_ANTI_DETECTION
		bool block_outgoing_super_jump_flag = true;
#endif
		// CPhysicalScriptGameStateDataNode
		bool block_outgoing_veh_godmode_flag = true;
		// CPlayerAppearanceDataNode
		uint32_t phone_mode_spoof = -1;

		DetourHook CNetObjVehicle_GetVehicleMigrationData_hook;
		rage::ObjectId tp_obj_id = NETWORK_INVALID_OBJECT_ID;
		rage::ObjectId tp_target_id = NETWORK_INVALID_OBJECT_ID;
		v3 tp_destination;

		DetourHook set_value_from_keyboard_hook;
		DetourHook set_value_from_mkb_axis_hook;

		DetourHook CTextFile_Get_hook;
		DetourHook CTextFile_GetInternal_hook;
		bool reveal_labels = false;
		std::unordered_map<Hash, std::string> label_replacements = {};
		bool owo_labels = false;
		bool chat_label_timestamps = false;
		bool can_modify_chat_labels = false;
		[[nodiscard]] static bool doesTextLabelExist(uint32_t hash);
		[[nodiscard]] static const char* getLabelText(uint32_t hash);
		[[nodiscard]] static std::string getLabelTextSafe(uint32_t hash);

		bool bypass_profanity_filter = false;
		bool bypass_profanity_filter_only_team = false;
		[[nodiscard]] bool shouldBypassProfanityFilter() const;
		[[nodiscard]] bool shouldBypassProfanityFilter(bool team_chat) const;
		[[nodiscard]] static std::string censorChatMessage(const std::string& message);
		static void censorChatMessageImpl(std::string& message);
		[[nodiscard]] static std::string makeUncensoredChatMessage(const std::string& _msg);
		[[nodiscard]] static bool isProfanityFilterBypass(std::string message);
		[[nodiscard]] static bool isCharFilterBypass(const std::string& message);

		DetourHook received_chat_message_hook;
		bool force_receive_chat = true;
		bool mock_players[MAX_PLAYERS + 1] = { 0 };
		bool owoify_players[MAX_PLAYERS + 1] = { 0 };

		bool owoify_my_outgoing_messages{};

		DetourHook scaleform_is_key_pressed_1_hook;
		uint16_t emulate_chat_message_backspace_press = 0;

		DetourHook is_chat_character_valid_hook;
		bool bypass_character_filter = false;
		bool bypass_character_filter_only_team = false;

		DetourHook send_CMsgTextMessage_hook;
		bool exclude_ad = false;

		DetourHook CNetworkTextChat_SubmitText_hook;
		bool force_send_chat = FORCE_SEND_MESSAGES_BY_DEFAULT;

		DetourHook CMultiplayerChat_UpdateInputWhenTyping_hook;

		DetourHook rage_netEventMgr_ProcessEventData_hook;
		bool notify_on_commendation = false;
		bool ignore_crash_n1_when_coloading = true;
		std::vector<NetworkedSound> processed_sounds;

		DetourHook CEventGroupScriptNetwork_DispatchEvent_hook;
		std::unordered_map<int32_t, std::pair<int, std::unique_ptr<int[]>>> last_script_event_map = {};
		bool toast_love_letter_kick = false;

		DetourHook rage_netPlayerMgrBase_SendBuffer_hook;

		DetourHook rage_fwArchetypeManager_GetArchetypeFromHashKey_hook;
		Spinlock first_model_info_request_times_mtx = {};
		std::unordered_map<Hash, time_t> first_model_info_request_times = {};
		[[nodiscard]] static CBaseModelInfo* getModelInfo(Hash modelHash) noexcept;

		const char* executing_script_name = nullptr;
		bool block_background_script = false;
		bool disable_restricted_areas = false;
		DetourHook script_vm_hook;
		DetourHook script_vm_switch_hook;
		DetourHook script_thread_error_kill_1_hook;
		DetourHook script_thread_error_kill_2_hook;
		//detour_hook script_vm_continue_hooks[41];

		DetourHook camFrame_copy_hook;
		float fov_override = FLT_MAX;
		//float current_fov;

		DetourHook CPhysical_TestNoCollision_hook;

		DetourHook CProjectileManager_CreateProjectile_hook;
		bool patch_projectile_pool = true;

		DetourHook rlGamerHandle_from_friend_index_hook;
		bool replace_friends_rid = false;
		uint64_t replacement_rid = 0;

		bool block_bail_rid = true;
		bool block_bail_crew = true;
		bool block_bail_spectating = false;
		bool block_bail_other = true;

		DetourHook rage_rlGetGamerStateTask_ParseResults_hook;

		bool disable_matchmaking = false;
		uint8_t magnet_matchmaking = 0;
		uint8_t magnet_matchmaking_this_request = 0;
		bool matchmaking_join = false;
		rage::rlSessionInfo matchmaking_join_session{};
		//DetourHook rage_rlScMatchmaking_Find_hook;
		//DetourHook rage_rlScMatchmakingFindTask_ReadSession_hook;
		//DetourHook rage_rlScMatchmakingFindTask_ProcessSuccess_hook;

		DetourHook rage_rlScMatchmaking_Advertise_hook;
		DetourHook rage_rlScMatchmaking_Unadvertise_hook;
		DetourHook rage_netTransactor_SendResponse_rage_rlSessionDetailResponse_hook;
		bool player_magnet = false;
		std::vector<rage::rlScMatchmakingMatchId> session_advertisements{};

		DetourHook get_matchmaking_pool_hook;
		bool spoof_matchmaking_pool = false;
		int spoofed_matchmaking_pool;

		DetourHook CExtraContentManager_GetCRC_hook;
		bool spoof_extra_content_crc = false;
		bool fix_asset_hashes = false;
		uint32_t spoofed_extra_content_crc = 0x00057498;
		uint32_t legit_extra_content_crc = 0;

		std::unordered_map<hash_t, FlowEventReactionData> custom_model_sync_reactions{};
		bool ignore_crash_c1_when_coloading = true;
		bool ignore_crash_c3_when_coloading = true;
		bool ignore_crash_u1_when_coloading = true;
		bool ignore_crash_u2_when_coloading = true;
		bool ignore_crash_cs_when_coloading = true;
		bool ignore_crash_sr_when_coloading = true;
		bool ignore_crash_t6_when_coloading = true;
		DetourHook received_clone_create_ack_hook;
		DetourHook received_clone_sync_ack_hook;
		DetourHook received_clone_remove_ack_hook;
		DetourHook clone_pack_hook;
		DetourHook received_clone_create_hook;
		DetourHook received_clone_sync_hook;

		static constexpr auto footlettuce_model_from = ATSTRINGHASH("bkr_prop_biker_barstool_01");
		static constexpr auto footlettuce_model_to = ATSTRINGHASH("prop_thindesertfiller_aa");
		CNetGamePlayer* footlettuce_target = nullptr;
		CNetGamePlayer* nextgen_target = nullptr;
		Whitelist<CNetGamePlayer*> outgoing_train_create_exclusive{};
		Whitelist<CNetGamePlayer*> outgoing_player_sync_exclusive{};

		AssociatedWhitelist<compactplayer_t, rage::netGameEvent*> play_sound_whitelist{};
		AssociatedWhitelist<compactplayer_t, rage::netGameEvent*> stop_sound_whitelist{};

		Blamer train_sync_blamer{ 5000 };
		Blamer schizo_sync_blamer{ 5000 };
		Blamer veh_appearance_sync_blamer{ 5000 };

		//DetourHook send_clone_create_hook;
		DetourHook send_clone_sync_hook;
		//DetourHook send_clone_remove_hook;
		DetourHook rage_netSyncTree_Read_hook;
#ifdef STAND_DEV
		DetourHook rage_netSyncDataNode_Read_hook;
#endif
		bool block_player_model_swaps = false;
		PlayerExcludes block_player_model_swaps_excludes;
		DetourHook sync_can_apply_hook;

		DetourHook CNetObjVehicle_SetVehicleControlData_hook;

		DetourHook InviteMgr_AcceptInvite_hook;
		bool managed_invite = false;
		bool join_as_spectator = false;
		//DetourHook CNetworkSession_JoinSession_hook;
		bool managed_join = false;

		uint32_t ll_lube = 1;
		uint32_t override_mm_group = -1;
		DetourHook rage_playerDataMsg_SerMsgPayload_datExportBuffer_hook;

		DetourHook rage_netArrayHandlerBase_WriteUpdate_hook;
		Whitelist<const rage::netPlayer*> pools_closed_kick{};
		bool org_spy = false;

		DetourHook rage_rlTelemetry_Export_hook;
#ifdef STAND_DEBUG
		bool log_metrics = false;
#endif
#if HTTP_HOOK
		DetourHook rage_netHttpRequest_Update_hook;
#ifdef STAND_DEBUG
		bool log_http_requests = false;
#endif
		std::string redir_host;
#endif
#if CLEAR_BONUS_ON_DL
		DetourHook CTunables_OnCloudEvent_hook;
#endif

		DetourHook rage_netEventMgr_SendEvent_hook;
#ifdef STAND_DEBUG
		bool log_net_event_send = false;
#endif

		DetourHook rage_gameSkeleton_updateGroup_Update_hook;
		DetourHook network_bail_hook;
		DetourHook rage_sysDependencyScheduler_InsertInternal_hook;
		DetourHook network_can_access_multiplayer_hook;

		bool erase_objects_with_no_mi = true;
		ToastConfig erase_objects_with_no_mi_notify{ true };

		DetourHook parse_presence_event_hook;
		DetourHook rgsc_RgscPresenceManager_OnSocialClubEvent_hook; 
#ifdef STAND_DEBUG
		bool log_presence_event_scdll = false;
		bool log_presence_event_game = false;
#endif
		bool scdm_scnotify = true;
		ToastConfig scdm_toasts{};

		DetourHook rage_ropeManager_Remove_hook;

		bool block_job_invites = false;
		bool block_friend_stat_notifications = false;

		bool block_joins_from_anyone = false;
		bool block_joins_from_strangers = false;
		bool block_joins_from_known_advertisers = true;
		bool block_joins_from_removed_players = false;
		bool block_joins_from_non_whitelisted = false;

		ToastConfig block_joins_from_anyone_notify{ true };
		ToastConfig block_joins_from_strangers_notify{ true };
		ToastConfig block_joins_from_known_advertisers_notify{ true };
		ToastConfig block_joins_from_removed_players_notify{ true };
		ToastConfig block_joins_from_non_whitelisted_notify{ true };
		ToastConfig block_joins_player_join_reactions_notify{ true };

		std::vector<std::pair<std::string, GamerIdentifierLatest>> removed_players{};
		DetourHook CNetworkSession_OnSessionEvent_hook;

		DetourHook CNetworkSession_OnHandleJoinRequest_hook;
		int32_t host_bj_message = 1;

		DetourHook rage_netIceSession_GetAdditionalLocalCandidates_hook;
		bool force_relay_connections = false;

		DetourHook get_visual_config_float_hook;
		DetourHook get_visual_config_float_with_directory_hook;
		DetourHook get_visual_config_colour_hook;
		std::unordered_map<std::string, float> visual_config_overrides{};

		DetourHook camFactory_CreateObject_hook;
		bool disable_cam_shake = false;

		DetourHook some_player_sync_hook;

		DetourHook rage_aiTaskTree_UpdateTask_hook;
		uint8_t graceful_landing = 0;

		DetourHook net_event_error_pre_memmove_hook;

		DetourHook CControl_StartPlayerPadShake_hook;

		DetourHook rage_FriendsReaderTask_Complete_hook;

		DetourHook aes_initfile_hook;
		DetourHook aes_decrypt_callsite_hook;
		DetourHook CModelInfo_AddVehicleModel_hook;
		DetourHook CNetObjVehicle_GetVehicleCreateData_hook;
		bool spoof_custom_models = true;

#ifdef STAND_DEBUG
		DetourHook rage_netMessage_WriteHeader_hook;
		bool log_packet_write = false;
#endif

	public:
		explicit Hooking();

		void initSpoofedClan();

		void init();
		void deinit();

		void setTargets();
		void performChecks();

		[[nodiscard]] std::vector<DetourHook*> getMinimalHooks();
		[[nodiscard]] std::vector<DetourHook*> getPassiveHooks();
		[[nodiscard]] std::vector<DetourHook*> getMainHooks();

		static void mergeHooks(std::vector<DetourHook*>& a, const std::vector<DetourHook*>& b)
		{
			a.insert(a.end(), b.begin(), b.end());
		}

		[[nodiscard]] std::vector<DetourHook*> getAllHooks();
		[[nodiscard]] std::vector<DetourHook*> getNonPassiveHooks();
		[[nodiscard]] std::vector<DetourHook*> getHookedHooks();

		void batchCreateHooks(const std::vector<DetourHook*>& hooks);
		void batchEnableHooks(const std::vector<DetourHook*>& hooks);
		void batchDisableHooks(const std::vector<DetourHook*>& hooks);
		void batchRemoveHooks(const std::vector<DetourHook*>& hooks);
		void batchDisableAndRemoveHooks(const std::vector<DetourHook*>& hooks);

		void hookWndproc();
		void unhookWndproc();

		void disableAll();
	};

	inline Hooking g_hooking{};
}
