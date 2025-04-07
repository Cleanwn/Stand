#pragma once

#include "gta_fwddecl.hpp"
#include "natives_decl.hpp"

namespace Stand
{
	using void_func_no_args_t = void(*)();
	using aes_func_t = bool(__fastcall*)(void* inst, void* data, unsigned int size);

	using CTheScripts_GetGUIDFromEntity_t = int(__fastcall*)(const rage::fwEntity&);

	using create_and_send_pickup_rewards_event_t = void(__fastcall*)(unsigned int player_bitflags, int reward_hash);
	using create_and_send_increment_stat_event_t = void(__fastcall*)(int stat_hash, int amount, CNetGamePlayer* recipient);

	using CExplosionEvent_Trigger_t = void(__fastcall*)(void* explosionArgs, CProjectile* pProjectile);

	using read_bitbuffer_dword_t = bool(__fastcall*)(rage::datBitBuffer*, uint32_t& out, unsigned int bits);
	using rage_datBitBuffer_ReadBits_t = bool(__fastcall*)(rage::datBitBuffer*, void* dst, unsigned int numBits, int dstBitOffset);
	using rage_datBitBuffer_WriteBits_t = bool(__fastcall*)(rage::datBitBuffer*, const void* src, unsigned int numBits, int srcBitOffset);

	using rage_datBitBuffer_WriteUnsigned_t = void(__fastcall*)(uint8_t* data, unsigned int val, int bits, int offset);

	using rage_datImport_netPeerAddress_t = bool(__fastcall*)(rage::datBitBuffer&, rage::netPeerAddress&);

	using send_net_event_ack_t = void(__fastcall*)(rage::netEventMgr* mgr, CNetGamePlayer* sender, CNetGamePlayer* receiver, uint32_t event_index, uint32_t event_handled_bitset);

	using chat_box_input_handler_t = char(__fastcall*)(CMultiplayerChat* a1);
	using CMultiplayerChat_OnEvent_t = void(__fastcall*)(CMultiplayerChat* thisptr, void* unused_CNetworkTextChat, const rage::rlGamerHandle& sender, const char* message, bool in_team_chat);
	using CMultiplayerChat_SetFocus_t = void(__fastcall*)(CMultiplayerChat* _this, int iVisibility, uint8_t eType);
	using rage_fwProfanityFilter_AsteriskProfanity_t = char*(__fastcall*)(rage::fwProfanityFilter* _this, char* message, uint8_t flags);
	using CNewHud_UpdateImeText_t = bool(__fastcall*)();

	using CVehicle_SetGravityForWheellIntegrator_t = __int64(__fastcall*)(CVehicle* ptr, float multiplier);

	using CPlayerListMenu_HandleContextOption_t = bool(__fastcall*)(CPlayerListMenu*, const int32_t* contextOption);

	using find_object_by_id_t = rage::netObject*(*)(rage::netObjectMgrBase* thisptr, uint16_t object_id, bool can_delete_be_pending);
	using rage_fwArchetype_GetFragType_t = rage::fragType*(__fastcall*)(rage::fwArchetype*);

	using CNetworkSession_KickPlayer_t = bool(__fastcall*)(CNetworkSession*, const rage::rlGamerHandle& pPlayerToKick, uint32_t session_index, uint32_t kickReason, int nComplaints, const rage::rlGamerHandle& hComplainer);
	using CBlacklistedGamers_BlacklistGamer_t = void(__fastcall*)(CBlacklistedGamers*, const rage::rlGamerHandle&, int nReason);

	using CRagdollRequestEvent_Trigger_t = void(__fastcall*)(uint16_t pedID);
	using CAlterWantedLevelEvent_Trigger_t = void(__fastcall*)(rage::netPlayer* player, int32_t newWantedLevel, int32_t delay, uint8_t causedByPlayerPhysicalIndex);
	using CWeaponDamageEvent_Trigger_t = void(__fastcall*)(
		CEntity* pParentEntity,
		CPed* pHitEntity,
		const v3* worldHitPosition,
		int hitComponent,
		bool bOverride,
		uint32_t weaponHash,
		float weaponDamage,
		int tyreIndex,
		int suspensionIndex,
		uint32_t damageFlags,
		uint32_t actionResultId,
		uint16_t meleeId,
		uint32_t nForcedReactionDefinitionId,
		bool hitEntityWeapon,
		bool hitWeaponAmmoAttachment,
		bool silenced,
		bool firstBullet,
		const v3* hitDirection
	);

	using CRotaryWingAircraft_BreakOffTailBoom_t = void(__fastcall*)(CRotaryWingAircraft* _this, int eBreakingState);
	using CVehicleDamage_BreakOffWheel_t = void(__fastcall*)(CVehicleDamage* _this, int wheelIndex, float ptfxProbability, float deleteProbability, float burstTyreProbability, bool dueToExplosion, bool bNetworkCheck);
	using CPlaySoundEvent_TriggerPos_t = void(__fastcall*)(const v3* vPosition, uint32_t setNameHash, uint32_t soundNameHash, uint8_t soundID, CGameScriptId* scriptID, int nRange);
	using CPlaySoundEvent_TriggerEnt_t = void(__fastcall*)(const CEntity* pEntity, unsigned int setNameHash, unsigned int soundNameHash, unsigned __int8 soundID, CGameScriptId* scriptId, int nRange);
	using CGiveControlEvent_Trigger_t = void(__fastcall*)(const rage::netPlayer* player, rage::netObject* pObject, rage::eMigrationType migrationType);
	using CProjectile_TriggerExplosion_t = void(__fastcall*)(CProjectile* _this, const uint32_t iRandomDelayMax);
	using CAudioBankRequestEvent_Trigger_t = void(__fastcall*)(unsigned int bankHash, CGameScriptId* scriptId, bool bIsRequest, unsigned int playerBits);
#if false
	using CFireEvent_Trigger_t = void(__fastcall*)(void* fire);
#endif

	using CAircraftDamageBase_BreakOffSection_t = void(__fastcall*)(CAircraftDamage* _this, CPlane* pParent, int nHitSection, bool bDisappear, bool bNetworkAllowed);
	using CNetworkPtFXEvent_Trigger_t = void(__fastcall*)(
		unsigned int* ptfxhash,
		unsigned int* ptfxassethash,
		const CEntity* pEntity,
		int boneIndex,
		const rage::Vector3* vfxpos,
		const rage::Vector3* vfxrot,
		float* scale,
		bool* invertAxes,
		float* r,
		float* g,
		float* b,
		float* alpha,
		bool ignoreScopeChanges
	);
	using CStopSoundEvent_Trigger_t = void(__fastcall*)(uint8_t soundID);

	using draw_line_t = void(*)(const rage::Vector2& pos1, const rage::Vector2& pos2);

	//using get_card_mgr_t = void*(*)(uint64_t, unsigned int);

	using send_session_info_request_t = bool(*)(rage::rlGamerHandle* gamerHandles, unsigned int count);
	//using dispatch_event_t = void(*)(EventDispatcher*, rage::rlPresenceEvent*); // might be more than just rage::rlPresenceEvent
	using InviteMgr_HandleJoinRequest_t = void(__fastcall*)(InviteMgr*, rage::rlSessionInfo*, rage::rlGamerInfo* invitee, rage::rlGamerHandle* inviter, unsigned int inviteFlags);

	using rage_rlSessionManager_QueryDetail_t = bool(__fastcall*)(int netMode, unsigned int channelId, int timeoutMs, int maxAttempts, int numResultsBeforeEarlyOut, bool solicited, int unk_set_to_3, const rage::rlGamerHandle& targetHandle, const rage::rlSessionInfo* sessionInfos, unsigned int numSessions, rage::rlSessionDetail* details, unsigned* numResults, rage::netStatus* status);

	using rage_netPeerAddress_Export_t = bool(__fastcall*)(const rage::netPeerAddress*, char* binary_out, int binary_out_size_bytes, unsigned int* outBytesWritten);

	using rgsc_Script_CallFunction_t = void(__fastcall*)(void* info);

	using send_MsgTextMessage_t = bool(__fastcall*)(rage::netConnectionManager*, int32_t cxnId, MsgTextMessage*, uint32_t sendFlags, rage::netSequence* frameSeq); // sms
	using send_netComplaintMsg_t = bool(__fastcall*)(rage::netConnectionManager*, const rage::netAddress&, uint32_t channelId, const rage::netComplaintMsg*, uint32_t);

	using rage_rlScMatchmaking_Find_t = bool(__fastcall*)(int localGamerIndex, unsigned int min_available_slots, const rage::rlMatchingFilter& filter, unsigned int maxResults, rage::rlSessionInfo* outResults, unsigned int* outNumResults, rage::netStatus* outStatus);

	using rage_rlClan_GetPrimaryMembership_t = rage::rlClanMembershipData*(__fastcall*)(int localGamerIndex);

	using get_my_rlGamerInfo_for_chat_t = rage::rlGamerInfo*(__fastcall*)();

	using rage_snSession_HandleRemoveGamerCmd_t = void(__fastcall*)(rage::snSession*, rage::snPeer*, rage::snMsgRemoveGamersFromSessionCmd*);

	// rage::strIndex *__fastcall rage::strPackfileManager::RegisterIndividualFile(rage::strIndex* result, const char* file, bool quitOnBadVersion, const char* relativePath, bool quitIfMissing)
	using register_file_t = uint32_t*(__fastcall*)(uint32_t* outTextureId, const char* path, bool unk1, const char* optFilename, bool errorsAreFatal, bool unk2);

	using rage_fwEntity_GetAttachedTo_t = rage::fwEntity*(__fastcall*)(rage::fwEntity*);

	using CVisualSettings_LoadAll_t = bool(*)(); // Actually takes a CVisualSettings*/thisptr, but that parameter is unused.

	using rage_rlScPresence_GetAttributesForGamer_t = bool(__fastcall*)(int localGamerIndex, const rage::rlGamerHandle& gamerHandle, rage::rlScPresenceAttribute* attrs, int numAttrs, rage::netStatus* status);

	using CPauseMenu_SetMenuPreference_t = bool(__fastcall*)(int iPref, int iValue);
	using CProfileSettings_Set_t = void(__fastcall*)(CProfileSettings*, uint32_t id, uint32_t value);
	using rage_rlSetLanguage_t = void(__fastcall*)(uint32_t language);

	using CPauseMenu_UpdateProfileFromMenuOptions_t = void(__fastcall*)(bool languageReload);

	using rage_netConnectionManager_SendConnectionless_t = bool(__stdcall*)(rage::netConnectionManager*, const rage::netAddress&, uint32_t channelId, const void* bytes, uint32_t numBytes, uint32_t sendFlags);

	using rage_netConnection_QueueFrame_t = bool(__fastcall*)(rage::netConnection*, const void* bytes, uint32_t numBytes, uint32_t sendFlags, rage::netSequence* frameSeq);
	using rage_netConnectionManager_CreateConnection_t = rage::Cxn*(__fastcall*)(rage::netConnectionManager*, rage::netEndpoint*, uint32_t channelId);
	using CNetworkSession_BuildJoinRequest_t = bool(__fastcall*)(CNetworkSession*, char* pData, unsigned int nMaxSize, unsigned int* nSizeOfData, unsigned int joinFlags);
	using rage_netConnectionManager_DestroyConnection_t = bool(__fastcall*)(rage::netConnectionManager*, rage::Cxn*);
	using rage_netRelay_GetMyRelayAddress_t = const rage::netAddress*(__fastcall*)();
	using rage_snMsgJoinRequest_Export_t = bool(__fastcall*)(void*, void* buf, int sizeofBuf, unsigned int* size);
	using rage_snMsgAddGamerToSessionBase_SerMsgPayload_datImportBuffer_t = bool(__fastcall*)(rage::datImportBuffer&, rage::snMsgAddGamerToSessionBase&);
	using rage_netConnectionManager_OpenTunnel_t = bool(__fastcall*)(rage::netConnectionManager*, const rage::rlGamerHandle&, const rage::netPeerAddress&, rage::netTunnelDesc&, void* tunnelRqst, rage::netStatus*);
	using rage_netConnectionManager_Send_t = bool(__fastcall*)(rage::netConnectionManager*, int32_t cxnId, const void* bytes, uint32_t numBytes, uint32_t sendFlags, rage::netSequence* frameSeq);

	using CVehicleModelInfo_ctor_t = void(__fastcall*)(CVehicleModelInfo*);
	using rage_fwArchetypeManager_RegisterStreamedArchetype_t = uint16_t(__fastcall*)(rage::fwArchetype*, int mapTypeDefIndex);

	using CExtraContentManager_AddContentFolder_t = bool(__fastcall*)(CExtraContentManager*, const char* path);
	using CExtraContentManager_LoadContent_t = void(__fastcall*)(CExtraContentManager*, bool executeChangeSet, bool executeEarlyStartup);

	using remove_known_ref_t = void(__fastcall*)(rage::fwRefAwareBase*, void** ppReference);

	using CNetworkAssetVerifier_RefreshEnvironmentCRC_t = void(__fastcall*)(CNetworkAssetVerifier* _this);

	using CTextConversion_FormatIntForHumans_t = void(__fastcall*)(__int64 iValue, char* pOutValue, int iLength, const char* pszPrefix, bool bAddCurrencyDeliminator);

	using rage_netIceSessionOffer_ctor_t = void(__fastcall*)(rage::netIceSessionOffer&);
	using rage_netIceSessionOffer_SerMsgPayload_datImportBuffer_t = bool(__fastcall*)(rage::datImportBuffer& bb, rage::netIceSessionOffer& msg);
	using rage_netIceSessionAnswer_SerMsgPayload_datImportBuffer_t = bool(__fastcall*)(rage::datImportBuffer& bb, rage::netIceSessionAnswer& msg);

	using CGameWorld_Remove_t = void(__fastcall*)(CEntity*, bool bIgnorePhysicsWorld, bool bSkipPostRemove);
	using CPedFactory_DestroyPed_t = bool(__fastcall*)(CPedFactory*, CPed*, bool cached);
	using vehicle_commands__DeleteScriptVehicle_t = void(__fastcall*)(CVehicle*);
	using CObjectPopulation_DestroyObject_t = void(__fastcall*)(CObject*, bool convertedToDummy);

	using rage_rlProfileStats_ReadStatsByGamer_t = bool(__fastcall*)(const int localGamerIndex, const rage::rlGamerHandle* gamers, int numGamers, rage::rlProfileStatsReadResults* results, rage::netStatus* status);

	using GFxValue_ObjectInterface_GetMember_t = bool(__fastcall*)(const void*, void*, const char*, GFxValue*, bool);
}
