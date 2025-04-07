#include "ComponentCrashPatch.hpp"

#include "AbstractEntity.hpp"
#include "CLightEntity.hpp"
#include "cloth.hpp"
#include "ComponentNetcode.hpp" // packet_src
#include "CPedDrawHandler.hpp"
#include "CPickup.hpp"
#include "crSkeleton.hpp"
#include "eEntityType.hpp"
#include "FlowEvent.hpp"
#include "grmShaderGroup.hpp"
#include "gta_custom_shader_effect.hpp"
#include "gta_ped.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "Label.hpp"
#include "phBoundComposite.hpp"
#include "phConstraint.hpp"
#include "pointers.hpp"
#include "Util.hpp"
#include "VehicleGadgets.hpp"
#include "VehicleType.hpp"

#include "ComponentImpl.hpp"

namespace Stand
{
	static int32_t* CPopCycle_sm_nCurrentDaySubdivision;

	static DetourHook report_error_wrap_hook{ "X3" };
	static DetourHook CPopCycle_UpdateCurrZone_hook{ "X4" };
	static DetourHook unk_cherax_crash_hook{ "X5" };
	static DetourHook unk_cherax_crash_2_hook{ "X6" };
	static DetourHook CVehicle_SlideMechanicalPart_hook{ "X7" };
	static DetourHook CPed_SwitchToRagdollInternal_hook{ "X8" };
	static DetourHook rage_crSkeleton_GetGlobalMtx_hook{ "X9" };
	static DetourHook CTorsoIkSolver_ApplyPointTorsoInDirection_hook{ "XA" };
	static DetourHook CLightEntity_GetWorldMatrix_hook{ "XB" };
	static DetourHook unk_izuku_crash_hook{ "XC" };
	static DetourHook rage_grcProgram_SetTextureResourcesUsingVectorDXAPICall_hook{ "XD" };
	static DetourHook ped_component_draw_thingy_hook{ "XE" };
	static DetourHook draw_object_hook{ "XF" };
	static DetourHook CCustomShaderEffectVehicleType_RestoreModelInfoDrawable_hook{ "XG" };
	static DetourHook update_ped_hair_texture_hook{ "XH" };
	static DetourHook CTaskHeliPassengerRappel_do_something_hook{ "XI" };
	static DetourHook rage_ComponentInfoManager_CreateComponentInfo_hook{ "XJ" };
	static DetourHook unk_related_to_object_despawning_hook{ "XK" };
	static DetourHook something_related_to_vehicle_parachutes_hook{ "XL" };
	static DetourHook CObjectFragmentDrawHandler_do_something_hook{ "XM" };
	static DetourHook CTrain_ValidateLinkedLists_hook{ "XN" };
	static DetourHook rage_characterClothController_SkinMesh_hook{ "XO" };
	static DetourHook rage_characterClothController_Update_hook{ "XP" };
	static DetourHook CPhysical_AttachToPhysicalBasic_hook{ "XQ" };
	static DetourHook CPhysical_AttachToPhysicalUsingPhysics_hook{ "XR" };
	static DetourHook CPickup_AssignCustomArchetype_hook{ "XS" };
	//static DetourHook rage_phConstraintMgr_AllocateAndConstruct_hook{ "XT" };
	static DetourHook rage_phConstraintBase_ctor_hook{ "XT" };
	static DetourHook CGameScriptHandlerMgr_AddToReservationCount_hook{ "XU" };
	static DetourHook CDynamicEntity_GetBoneIndexFromBoneTag_hook{ "XV" };
	static DetourHook CBodyRecoilIkSolver_PreIkUpdate_hook{ "XW" };
	static DetourHook ARTFeedbackInterfaceGta_onBehaviourFailure_hook{ "XX" };
	static DetourHook CTaskVehicleMountedWeapon_ComputeSearchTarget_hook{ "XY" };

	static bool reported_fatal_error = false;

	static void __fastcall report_error_wrap(uint32_t a1)
	{
		SOUP_IF_UNLIKELY (!g_gui.doesRootStateAllowCrashPatches())
		{
			return COMP_OG(report_error_wrap)(a1);
		}
		if (packet_src.isValid())
		{
			//packet_src.getAndApplyReactions(FlowEvent::SE_CRASH, "E0");
			Util::onPreventedCrash("E0", Util::to_padded_hex_string(a1), packet_src);
		}
		else if (!reported_fatal_error)
		{
			Exceptional::report(fmt::format("Fatal Game Error (Code {})", Util::to_padded_hex_string(a1)), LANG_GET("ERR_T_GME"));
			reported_fatal_error = true;
		}
	}

	static void __fastcall CPopCycle_UpdateCurrZone()
	{
		__try
		{
			if (*CPopCycle_sm_nCurrentDaySubdivision >= 12)
			{
				*CPopCycle_sm_nCurrentDaySubdivision %= 12;
				Util::onPreventedCrash(CODENAME(CPopCycle_UpdateCurrZone));
			}
		}
		__EXCEPTIONAL()
		{
		}
		__try
		{
			COMP_OG(CPopCycle_UpdateCurrZone)();
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	// void __fastcall CPhysical::DetachFromParentAndChildren(CPhysical *this, unsigned __int16 nDetachFlags)
	static void __fastcall unk_cherax_crash(int64_t* a1, unsigned __int16 a2)
	{
		__try
		{
			COMP_OG(unk_cherax_crash)(a1, a2);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	// void __fastcall CPopSchedule::SetOverridePedGroup(CPopSchedule* this, unsigned int overridePedGroup, unsigned int overridePercentage)
	static void __fastcall unk_cherax_crash_2(__int64 a1, int a2, int a3)
	{
		__try
		{
			return COMP_OG(unk_cherax_crash_2)(a1, a2, a3);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(unk_cherax_crash_2));
		}
	}

	// Invalid bone index when doing deluxo flight with some vehicles, like kuruma.
	// I'm not aware of this being networkable.
	static void __fastcall CVehicle_SlideMechanicalPart(CVehicle* _this, int iBoneIndex, float fDesiredOffset, float* rfCurrentOffset, v3* vSlideDirection, float fMovementSpeed)
	{
		SOUP_IF_UNLIKELY (iBoneIndex < 0)
		{
			SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
			{
				if (!g_comp_crashpatch.silently_block_invalid_mechanical_part)
				{
					Util::onPreventedCrash(CODENAME(CVehicle_SlideMechanicalPart));
				}
				return;
			}
		}
		return COMP_OG(CVehicle_SlideMechanicalPart)(_this, iBoneIndex, fDesiredOffset, rfCurrentOffset, vSlideDirection, fMovementSpeed);
	}

	// Out-of-bounds boneIdx e.g. when a_c_poodle dies or starts to ragdoll, because it's missing a weapon bone. See scripts/no-weapon-bone.pluto.
	// That ends up throwing in CPedWeaponManager::SwitchToRagdoll because of ->GetUndamagedEntity()->GetBoundMatrix() on nullptr.
	// We catch it here.
	static void __fastcall CPed_SwitchToRagdollInternal(CPed* a1)
	{
		__try
		{
			COMP_OG(CPed_SwitchToRagdollInternal)(a1);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(CPed_SwitchToRagdollInternal));
		}
	}

	static bool can_catch_out_of_bounds_boneIdx = false;

	struct OutOfBoundsBoneIdx {};

	static void __fastcall rage_crSkeleton_GetGlobalMtx(rage::crSkeleton* a1, unsigned int boneIdx, rage::Mat34V* outMtx)
	{
		SOUP_IF_UNLIKELY (boneIdx >= a1->num_bones)
		{
			SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
			{
				if (can_catch_out_of_bounds_boneIdx)
				{
					throw OutOfBoundsBoneIdx();
				}
				boneIdx = (a1->num_bones - 1);
				Util::onPreventedCrash(CODENAME(rage_crSkeleton_GetGlobalMtx));
			}
		}
		__try
		{
			COMP_OG(rage_crSkeleton_GetGlobalMtx)(a1, boneIdx, outMtx);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	// Catch out-of-bounds boneIdx here, can occur e.g. when shooting weapons as a fish.
	// Also seems to be a part of Nightfall's "Luna" crash.
	static void __fastcall CTorsoIkSolver_ApplyPointTorsoInDirection(__int64 a1)
	{
		can_catch_out_of_bounds_boneIdx = true;
		try
		{
			COMP_OG(CTorsoIkSolver_ApplyPointTorsoInDirection)(a1);
		}
		catch (OutOfBoundsBoneIdx&)
		{
			Util::onPreventedCrash(CODENAME(CTorsoIkSolver_ApplyPointTorsoInDirection));
		}
		can_catch_out_of_bounds_boneIdx = false;
	}

	static bool CLightEntity_GetWorldMatrix(CLightEntity* _this, rage::Matrix34& mat, /*C2dEffect*/ void* pEffect, /*CLightAttr*/ void* lightData)
	{
		__try
		{
			if (!pEffect
				|| (_this->Get2dEffectType() == ET_LIGHT
					&& !lightData
					)
				)
			{
				SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
				{
					Util::onPreventedCrash(CODENAME(CLightEntity_GetWorldMatrix));

					// LightEntityMgr::Remove
					{
						pointers::CGameWorld_Remove(_this, false, false);
						delete _this;
					}
				}

				return false;
			}

			return COMP_OG(CLightEntity_GetWorldMatrix)(_this, mat, pEffect, lightData);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
		return false;
	}

	// void __fastcall rage::netObjectMgrBase::Update(rage::netObjectMgrBase *this, bool bUpdateNetworkObjects)
	//
	// This catches an exception when trying to apply a force to an object where HAS_OBJECT_BEEN_BROKEN is true
	// Possibly also when trying to apply a force on an invalid entity handle
	// Not sure what "Izuku" does
	//
	// We also catch an exception here in rage::netSyncTree::Update when GetSyncData returned a nullptr.
	static void __fastcall unk_izuku_crash(rage::netObjectMgrBase* a1, bool bUpdateNetworkObjects)
	{
		__try
		{
			COMP_OG(unk_izuku_crash)(a1, bUpdateNetworkObjects);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	// This likes to crash sometimes in D3D11.dll, unsure why.
	static void __fastcall rage_grcProgram_SetTextureResourcesUsingVectorDXAPICall(void* _this, int eType)
	{
		__try
		{
			COMP_OG(rage_grcProgram_SetTextureResourcesUsingVectorDXAPICall)(_this, eType);
		}
		__EXCEPTIONAL()
		{
		}
	}

	// void __fastcall rage::grmShaderGroup::SetVar(rage::grmShaderGroup* this, rage::__grmShaderGroupVar var, const rage::grcTexture* tex)
	static void __fastcall ped_component_draw_thingy(rage::grmShaderGroup* a1, int a2, __int64 a3)
	{
		__try
		{
			if (a1->var_array)
			{
				COMP_OG(ped_component_draw_thingy)(a1, a2, a3);
			}
			else
			{
				Util::onPreventedCrash(CODENAME(ped_component_draw_thingy));
			}
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(ped_component_draw_thingy));
		}
	}

	// void __fastcall rage::grcVertexProgram::SetStructuredBuffer(unsigned int address, const rage::grcBufferD3D11Resource* pBuffer)
	static void __fastcall draw_object(unsigned int a1, __int64 a2)
	{
		__try
		{
			COMP_OG(draw_object)(a1, a2);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(draw_object));
		}
	}

	// Related to CVehicleStreamRequestGfx/CVehicleStreamRenderGfx.
	static bool __fastcall CCustomShaderEffectVehicleType_RestoreModelInfoDrawable(CCustomShaderEffectVehicleType* _this, rage::rmcDrawable* pDrawable)
	{
		__try
		{
			return COMP_OG(CCustomShaderEffectVehicleType_RestoreModelInfoDrawable)(_this, pDrawable);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
		return false;
	}

	// void __fastcall CPedVariationStream::SetPaletteTexture(CPed *pPed, int comp)
	static void __fastcall update_ped_hair_texture(CPed* a1, int a2)
	{
		__try
		{
			auto gfx = reinterpret_cast<CPedDrawHandler*>(a1->draw_handler)->stream_render_gfx;
			SOUP_IF_UNLIKELY (gfx != nullptr // game can handle gfx being nullptr
				&& gfx->custom_shader_effect == nullptr // but not the custom shader effect being nullptr
				)
			{
				SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
				{
					return;
				}
			}
			COMP_OG(update_ped_hair_texture)(a1, a2);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	// CTaskHeliPassengerRappel::ProcessPostMovement
	static bool __fastcall CTaskHeliPassengerRappel_do_something(__int64 a1)
	{
		__try
		{
			return COMP_OG(CTaskHeliPassengerRappel_do_something)(a1);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
		return false;
	}

	// Related to 2take1's "Fragment" crash, which spawns prop_fragtest_cnst_04 and breaks it.
	// That causes this function to be called within phMidphase::ProcessSelfCollisionNew with an invalid component index.
	// We force the component in-bound. It can't be nullptr because of the subsequent call to OBBTest.
	static rage::ComponentInfo* __fastcall rage_ComponentInfoManager_CreateComponentInfo(rage::ComponentInfoManager* _this, int component, const rage::Mat34V* curInstanceMatrix, const rage::Mat34V* lastInstanceMatrix, const rage::phBoundComposite* boundComposite)
	{
		SOUP_IF_UNLIKELY (boundComposite->bounds[component] == nullptr)
		{
			SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
			{
				component = 0;
				Util::onPreventedCrash(CODENAME(rage_ComponentInfoManager_CreateComponentInfo));
			}
		}
		return COMP_OG(rage_ComponentInfoManager_CreateComponentInfo)(_this, component, curInstanceMatrix, lastInstanceMatrix, boundComposite);
	}

	// __int64 __fastcall CObject::ProcessControl(CObject* this)
	static __int64 __fastcall unk_related_to_object_despawning(CObject* a1, __int64 a2, __int64 a3)
	{
		__try
		{
			return COMP_OG(unk_related_to_object_despawning)(a1, a2, a3);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
		}
		Util::onPreventedCrash(CODENAME(unk_related_to_object_despawning));
		__try
		{
			AbstractEntity::get(reinterpret_cast<CPhysical*>(a1)).removeFromPlaneOfExistence();
		}
		__EXCEPTIONAL()
		{
		}
		return 0;
	}

	// CAutomobile::ProcessCarParachute
	static void __fastcall something_related_to_vehicle_parachutes(void* a1)
	{
		__try
		{
			return COMP_OG(something_related_to_vehicle_parachutes)(a1);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(something_related_to_vehicle_parachutes));
		}
	}

	// rage::dlCmdBase *__fastcall CObjectFragmentDrawHandler::AddToDrawList(CObjectFragmentDrawHandler* this, CEntity* pBaseEntity, rage::fwDrawDataAddParams* pBaseParams)
	static __int64 __fastcall CObjectFragmentDrawHandler_do_something(__int64 CObjectFragmentDrawHandler, CEntity* a2, __int64 a3)
	{
		SOUP_IF_LIKELY (auto mi = g_hooking.getModelInfo(a2->archetype->hash);
			mi != nullptr
			&& pointers::rage_fwArchetype_GetFragType(mi) != nullptr
			)
		{
			__try
			{
				return COMP_OG(CObjectFragmentDrawHandler_do_something)(CObjectFragmentDrawHandler, a2, a3);
			}
			__EXCEPTIONAL()
			{
			}
		}
		else
		{
			// Not a valid model, remove this entity.
			__try
			{
				AbstractEntity::get(reinterpret_cast<CPhysical*>(a2)).removeFromPlaneOfExistenceForce();
				Util::onPreventedCrash(CODENAME(CObjectFragmentDrawHandler_do_something));
			}
			__EXCEPTIONAL()
			{
			}
		}
		return 0;
	}

	static void __fastcall CTrain_ValidateLinkedLists(CTrain* carriage)
	{
		__try
		{
			if (!carriage)
			{
				return;
			}

			// `carriage` has just been set to a train's linked_backward or linked_forward

			SOUP_IF_UNLIKELY (carriage->isBackwardLinkedTo(carriage))
			{
				SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
				{
					//Util::toast("Recursive backward linkage detected, trying to fix it.", TC_OTHER);
					Util::onPreventedCrash(LOC("CRSH_TRNLOOP"), g_hooking.train_sync_blamer.getBlame());
					carriage->linked_backward.reset();
				}
			}

			SOUP_IF_UNLIKELY (carriage->isForwardLinkedTo(carriage))
			{
				SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
				{
					//Util::toast("Recursive forward linkage detected, trying to fix it.", TC_OTHER);
					Util::onPreventedCrash(LOC("CRSH_TRNLOOP"), g_hooking.train_sync_blamer.getBlame());
					carriage->linked_forward.reset();
				}
			}

			return COMP_OG(CTrain_ValidateLinkedLists)(carriage);
		}
		__EXCEPTIONAL()
		{
		}
	}

	// Part of Nightfall's "Schizophrenic crash", transforms the player into cs_solomon, cs_josh, or cs_lazlow.
	static void rage_characterClothController_SkinMesh(rage::characterClothController* _this, const rage::crSkeleton* skeleton)
	{
		__try
		{
			for (auto& boneIdx : _this->m_BoneIndexMap)
			{
				SOUP_IF_UNLIKELY (boneIdx >= skeleton->num_bones)
				{
					//boneIdx = (skel->num_bones - 1);
					Util::onPreventedCrash(CODENAME(rage_characterClothController_SkinMesh), g_hooking.schizo_sync_blamer.getBlame());
					return;
				}
			}
			COMP_OG(rage_characterClothController_SkinMesh)(_this, skeleton);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	// Same as above.
	static void rage_characterClothController_Update(rage::characterClothController* _this, const rage::Vec3V* gravityV, float timeScale, const rage::crSkeleton* skeleton, float fTime, const rage::phBoundComposite* customBound, int* bonesIndices)
	{
		__try
		{
			for (auto& boneIdx : _this->m_BoneIndexMap)
			{
				SOUP_IF_UNLIKELY (boneIdx >= skeleton->num_bones)
				{
					SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
					{
						//boneIdx = (skel->num_bones - 1);
						Util::onPreventedCrash(CODENAME(rage_characterClothController_Update), g_hooking.schizo_sync_blamer.getBlame());
						return;
					}
				}
			}
			COMP_OG(rage_characterClothController_Update)(_this, gravityV, timeScale, skeleton, fTime, customBound, bonesIndices);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
	}

	[[nodiscard]] static bool validateAttachment(CPhysical* attacher, CPhysical* attachee)
	{
		SOUP_IF_UNLIKELY (!g_gui.doesRootStateAllowCrashPatches())
		{
			return true;
		}

		//Util::toast(fmt::format("Attaching {} to {}", attacher->toString(), attachee->toString()), TOAST_ALL);

		// This goes wrong because CTrailer often assumes its attach parent is CVehicle and tries to access its data,
		// e.g. CTrailer::DetachFromParent will try to access the vehicle gadgets of its attach parent.
		if (attacher->type == ENTITY_TYPE_VEHICLE
			&& static_cast<CVehicle*>(attacher)->vehicle_type == VEHICLE_TYPE_TRAILER
			&& attachee->type != ENTITY_TYPE_VEHICLE
			)
		{
			//Util::toast(PHSTR("Attaching a trailer to a non-vehicle? Nonsense."));
			Util::onPreventedCrash(CODENAME(CPhysical_AttachToPhysicalBasic));
			return false;
		}

		for (auto node = attacher; node; node = static_cast<CPhysical*>(node->GetChildAttachment()))
		{
			if (node == attachee)
			{
				//Util::toast(PHSTR("My new parent is already my child? Nonsense."), TOAST_ALL);
				Util::onPreventedCrash(LOC("CRSH_ATTLOOP"));
				return false;
			}
		}

		for (auto node = attacher; node; node = static_cast<CPhysical*>(node->GetSiblingAttachment()))
		{
			if (node == attachee)
			{
				//Util::toast(PHSTR("My new parent is already my sibling? Nonsense."), TOAST_ALL);
				Util::onPreventedCrash(LOC("CRSH_ATTLOOP"));
				return false;
			}
		}

		for (auto node = attachee; node; node = static_cast<CPhysical*>(node->GetAttachParentForced()))
		{
			if (node == attacher)
			{
				//Util::toast(PHSTR("My new child is already my parent? Nonsense."), TOAST_ALL);
				Util::onPreventedCrash(LOC("CRSH_ATTLOOP"));
				return false;
			}
		}

		return true;
	}

	static void __fastcall CPhysical_AttachToPhysicalBasic(CPhysical* _this, CPhysical* pPhysical, int16_t nEntBone, uint32_t nAttachFlags, const v3* pVecOffset, const void* pQuatOrientation, int16_t nMyBone)
	{
		SOUP_IF_LIKELY (validateAttachment(_this, pPhysical))
		{
			COMP_OG(CPhysical_AttachToPhysicalBasic)(_this, pPhysical, nEntBone, nAttachFlags, pVecOffset, pQuatOrientation, nMyBone);
		}
	}

	static bool __fastcall CPhysical_AttachToPhysicalUsingPhysics(CPhysical* _this, CPhysical* pPhysical, int16_t nParentBone, int16_t nMyBone, uint32_t nAttachFlags, const v3* pParentAnchorLocalSpace, const void* pQuatOrientation, const v3* pMyAnchorLocalSpace, float fStrength, bool bAllowInitialSeparation, float massInvScaleA, float massInvScaleB)
	{
		SOUP_IF_UNLIKELY (!validateAttachment(_this, pPhysical))
		{
			return false;
		}

		return COMP_OG(CPhysical_AttachToPhysicalUsingPhysics)(_this, pPhysical, nParentBone, nMyBone, nAttachFlags, pParentAnchorLocalSpace, pQuatOrientation, pMyAnchorLocalSpace, fStrength, bAllowInitialSeparation, massInvScaleA, massInvScaleB);
	}

	// Sometimes the game may try to apply a starting velocity on a pickup without a collider.
	// Not sure what exactly triggers this, something about killing/exploding peds who are holding RPGs while the renderer is overworked.
	// I tried detecting the exact conditions under which it would occur and then zero the velocity, but it would false-positive, so try-except it is.
	static void CPickup_AssignCustomArchetype(CPickup* _this)
	{
		__try
		{
			COMP_OG(CPickup_AssignCustomArchetype)(_this);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(CPickup_AssignCustomArchetype));
		}
	}

	// rage::phConstraintMgr::InsertAndReturnTemporaryPointer will not free its lock if this throws.
	/*static void* __fastcall rage_phConstraintMgr_AllocateAndConstruct(void* _this, const rage::phConstraintBase::Params& params)
	{
		__try
		{
			return COMP_OG(rage_phConstraintMgr_AllocateAndConstruct)(_this, params);
		}
		__EXCEPTIONAL()
		{
			//Util::onPreventedCrash(CODENAME(rage_phConstraintMgr_AllocateAndConstruct));
		}
		return nullptr;
	}*/

	// In case ctor fails, we should initialise pointers to nullptr so DisableManifolds and possibly other functions don't go insane.
	static void rage_phConstraintBase_ctor(rage::phConstraintBase* _this, const rage::phConstraintBase::Params& params)
	{
		switch (params.type)
		{
		case rage::phConstraintBase::ATTACHMENT:
			static_cast<rage::phConstraintAttachment*>(_this)->m_RotationConstraint = nullptr;
			break;

		case rage::phConstraintBase::FIXED:
			static_cast<rage::phConstraintFixed*>(_this)->m_TranslationManifold = nullptr;
			static_cast<rage::phConstraintFixed*>(_this)->m_RotationManifold = nullptr;
			break;

		case rage::phConstraintBase::FIXEDROTATION:
			static_cast<rage::phConstraintFixedRotation*>(_this)->m_RotationManifold = nullptr;
			break;

		case rage::phConstraintBase::PRISMATIC:
			for (auto*& pManifold : static_cast<rage::phConstraintPrismatic*>(_this)->m_Manifolds)
			{
				pManifold = nullptr;
			}
			break;
		}
		COMP_OG(rage_phConstraintBase_ctor)(_this, params);
	}

	// This is caught by rage::netObjectMgrBase::Update, but I don't like that hook.
	static void __fastcall CGameScriptHandlerMgr_AddToReservationCount(CGameScriptHandlerMgr* _this, /* CNetObjGame* */ void* scriptEntityObj)
	{
		__try
		{
			COMP_OG(CGameScriptHandlerMgr_AddToReservationCount)(_this, scriptEntityObj);
		}
		__EXCEPTIONAL()
		{
		}
	}

	static bool bone_index_must_be_in_bounds = false;

	// CBodyRecoilIkSolver::PreIkUpdate uses this function's result directly as an array index.
	// scripts/body-recoil-ik.lua
	static int32_t __fastcall CDynamicEntity_GetBoneIndexFromBoneTag(CDynamicEntity* _this, const /* eAnimBoneTag */ int32_t boneTag)
	{
		const auto boneIdx = COMP_OG(CDynamicEntity_GetBoneIndexFromBoneTag)(_this, boneTag);
		SOUP_IF_UNLIKELY (bone_index_must_be_in_bounds
			&& boneIdx == -1
			)
		{
			SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
			{
				Util::onPreventedCrash(CODENAME(CDynamicEntity_GetBoneIndexFromBoneTag));
				return 0;
			}
		}
		return boneIdx;
	}

	static void __fastcall CBodyRecoilIkSolver_PreIkUpdate(void /*CBodyRecoilIkSolver*/* _this, float deltaTime)
	{
		bone_index_must_be_in_bounds = true;
		__try
		{
			COMP_OG(CBodyRecoilIkSolver_PreIkUpdate)(_this, deltaTime);
		}
		__EXCEPTIONAL_IF(g_gui.doesRootStateAllowCrashPatches())
		{
		}
		bone_index_must_be_in_bounds = false;
	}

	// There is a weird interaction here where CTaskNMShot::m_pShotBy is a ped without a CPedIntelligence, causing a nullptr dereference in CTaskNMShot::BehaviourFailure.
	// The same may also happen in CTaskNMFlinch.
	// We can catch this via our rage::aiTaskTree::UpdateTask hook, but this causes memory corruption, which will crash if used often enough.
	// See scripts/CTaskNMShot.pluto.
	static int __fastcall ARTFeedbackInterfaceGta_onBehaviourFailure(void* _this)
	{
		__try
		{
			return COMP_OG(ARTFeedbackInterfaceGta_onBehaviourFailure)(_this);
		}
		__except (g_gui.doesRootStateAllowCrashPatches() ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{
			Util::onPreventedCrash(CODENAME(ARTFeedbackInterfaceGta_onBehaviourFailure));
		}
		return 0;
	}

	// This checks for nullptr only after it's been dereferenced.
	// We can't catch this in rage::aiTaskTree::UpdateTask because we could also get here from CPed::ProcessPostCamera.
	static void __fastcall CTaskVehicleMountedWeapon_ComputeSearchTarget(rage::Vector3* o_Target, CPed* in_Ped)
	{
		bool searchlight_is_not_nullptr = false;
		if (in_Ped->m_pMyVehicle.m_p)
		{
			if (auto vwm = in_Ped->m_pMyVehicle.m_p->m_pVehicleWeaponMgr)
			{
				for (int i = 0; i != vwm->m_iNumVehicleWeapons; ++i)
				{
					if (vwm->m_pVehicleWeapons[i]->GetType() == VGT_SEARCHLIGHT)
					{
						searchlight_is_not_nullptr = true;
						break;
					}
				}
			}
		}
		SOUP_IF_LIKELY (searchlight_is_not_nullptr)
		{
			return COMP_OG(CTaskVehicleMountedWeapon_ComputeSearchTarget)(o_Target, in_Ped);
		}
		Util::onPreventedCrash(CODENAME(CTaskVehicleMountedWeapon_ComputeSearchTarget));
	}

	void ComponentCrashPatch::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD_OPTIONAL(CODENAME(report_error_wrap), "B9 39 EC 8B 43 E8", [](soup::Pointer p)
		{
			p = p.add(6).rip();
			CHECK_EXISTING_HOOK(CODENAME(report_error_wrap), "48 83 EC 28 33");
			INIT_HOOK_OPTIONAL(report_error_wrap);
		});
		BATCH_ADD_OPTIONAL(CODENAME(CPopCycle_UpdateCurrZone), "2B C2 D1 F8 89 05 ? ? ? ? E8", [](soup::Pointer p)
		{
			CPopCycle_sm_nCurrentDaySubdivision = p.add(6).rip().as<int32_t*>();

			p = p.add(11).rip();
			CHECK_EXISTING_HOOK(CODENAME(CPopCycle_UpdateCurrZone), "48 83 EC 48 80");
			INIT_HOOK_OPTIONAL(CPopCycle_UpdateCurrZone);
		});
		BATCH_ADD_OPTIONAL_LONGJUMP_HOOK(unk_cherax_crash, "48 89 5C 24 08", "48 89 74 24 10 57 48 83 EC 20 48 8B 41 50 0F B7 F2 48 85 C0 74 04 48 8B 40 48");
		BATCH_ADD_OPTIONAL_LONGJUMP_HOOK(unk_cherax_crash_2, "44 89 81 4C 02", "00 00 89 91 48 02 00 00 C6 05 ? ? ? ? 01 C3");
		BATCH_ADD_OPTIONAL(CODENAME(CVehicle_SlideMechanicalPart), "34 00 00 80 BF 48 89 44 24 20 E8", [](soup::Pointer p)
		{
			p = p.add(11).rip();
			CHECK_EXISTING_HOOK(CODENAME(CVehicle_SlideMechanicalPart), "48 8B C4 48 89");
			INIT_HOOK_OPTIONAL(CVehicle_SlideMechanicalPart);
		});
		BATCH_ADD_OPTIONAL(CODENAME(CPed_SwitchToRagdollInternal), "0F 84 31 01 00 00 48 8B CE E8", [](soup::Pointer p)
		{
			// p is now inside of CEvent *__fastcall CPed::SwitchToRagdoll(CPed *this, CEvent *event)
			p = p.add(10).rip();
			CHECK_EXISTING_HOOK(CODENAME(CPed_SwitchToRagdollInternal), "48 8B C4 48 89");
			INIT_HOOK_OPTIONAL_LONGJUMP(CPed_SwitchToRagdollInternal);
		});
		BATCH_ADD_OPTIONAL_HOOK(rage_crSkeleton_GetGlobalMtx, "40 55 48 8D 6C", "24 A9 48 81 EC D0 00 00 00 4D 8B D0");
		BATCH_ADD_OPTIONAL_HOOK(CTorsoIkSolver_ApplyPointTorsoInDirection, "48 8B C4 55 53", "56 57 41 54 41 55 41 56 41 57 48 8D A8 78 FE FF FF 48 81 EC 48 02 00 00 48 8B 79 30");
		BATCH_ADD_OPTIONAL_HOOK(CLightEntity_GetWorldMatrix, "40 53 48 83 EC", "60 0F 29 74 24 50 49 8B C0");
		BATCH_ADD_OPTIONAL_LONGJUMP_HOOK(unk_izuku_crash, "48 89 5C 24 08", "48 89 74 24 10 57 48 83 EC 20 48 8B D9 84 D2 74 47");
		BATCH_ADD_OPTIONAL_LONGJUMP_HOOK(rage_grcProgram_SetTextureResourcesUsingVectorDXAPICall, "48 89 5C 24 10", "48 89 6C 24 18 48 89 74 24 20 57 41 54 41 55 41 56 41 57 48 83 EC 20 8B 81 A8 00 00 00 48 63 DA");
		BATCH_ADD_OPTIONAL(CODENAME(ped_component_draw_thingy), "4D 8B 45 70 48 8B 4B 10 8B D0 E8", [](soup::Pointer p)
		{
			p = p.add(11).rip();
			CHECK_EXISTING_HOOK(CODENAME(ped_component_draw_thingy), "85 D2 0F 84 83");
			INIT_HOOK_OPTIONAL(ped_component_draw_thingy);
		});
		BATCH_ADD_OPTIONAL_HOOK(draw_object, "48 83 EC 28 45", "33 C0 4C 63 D1 48 85 D2");
		BATCH_ADD_OPTIONAL_HOOK(CCustomShaderEffectVehicleType_RestoreModelInfoDrawable, "48 8B C4 48 89", "58 08 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D 68 A1 48 81 EC C0 00 00 00 33 DB")
		BATCH_ADD_OPTIONAL(CODENAME(update_ped_hair_texture), "48 89 AC 03 30 FF FF FF E8", [](soup::Pointer p)
		{
			p = p.add(9).rip();
			CHECK_EXISTING_HOOK(CODENAME(update_ped_hair_texture), "40 55 53 56 57");
			INIT_HOOK_OPTIONAL(update_ped_hair_texture);
		});
		BATCH_ADD_OPTIONAL_HOOK(CTaskHeliPassengerRappel_do_something, "48 8B C4 48 89", "58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 D8 FE FF FF");
		BATCH_ADD_OPTIONAL(CODENAME(rage_ComponentInfoManager_CreateComponentInfo), "8B D5 0F 29 4C 24 60 E8", [](soup::Pointer p)
		{
			p = p.add(8).rip();
			CHECK_EXISTING_HOOK(CODENAME(rage_ComponentInfoManager_CreateComponentInfo), "40 53 48 83 EC");
			INIT_HOOK_OPTIONAL(rage_ComponentInfoManager_CreateComponentInfo);
		});
		BATCH_ADD_OPTIONAL_HOOK(unk_related_to_object_despawning, "48 8B C4 48 89", "58 10 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D A8 48 FF FF FF 48 81 EC 90 01 00 00 0F 29 70 C8");
		BATCH_ADD_OPTIONAL_HOOK(something_related_to_vehicle_parachutes, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 54 48 81 EC E0 00 00 00 0F 29 70 E8");
		BATCH_ADD_OPTIONAL_HOOK(CObjectFragmentDrawHandler_do_something, "48 89 5C 24 08", "48 89 6C 24 18 56 57 41 54 41 56 41 57 48 81 EC 80 00 00 00 48 8B E9");
		BATCH_ADD_OPTIONAL(CODENAME(CTrain_ValidateLinkedLists), "49 8B 0E E8 ? ? ? ? 48 8D 9F ? ? 00 00 48 8B 0B", [](soup::Pointer p)
		{
			p = p.add(4).rip();
			CHECK_EXISTING_HOOK(CODENAME(CTrain_ValidateLinkedLists), "48 85 C9 0F");
			INIT_HOOK_OPTIONAL(CTrain_ValidateLinkedLists);
		});
		BATCH_ADD_OPTIONAL_HOOK(rage_characterClothController_SkinMesh, "40 55 53 56 57", "41 54 41 55 41 56 41 57 48 81 EC 48 01 00 00 48 8D 6C 24 50 48 0F BE 41 56");
		BATCH_ADD_OPTIONAL_HOOK(rage_characterClothController_Update, "48 8B C4 4C 89", "48 20 F3 0F 11 50 18 48 89 50 10");
		BATCH_ADD_OPTIONAL(CODENAME(CPhysical_AttachToPhysicalBasic), "48 89 44 24 20 E8 ? ? ? ? 80 7B 28 04", [](soup::Pointer p)
		{
			p = p.add(6).rip();
			CHECK_EXISTING_HOOK(CODENAME(CPhysical_AttachToPhysicalBasic), "48 85 D2 0F 84");
			INIT_HOOK_OPTIONAL(CPhysical_AttachToPhysicalBasic);
		});
		BATCH_ADD_OPTIONAL(CODENAME(CPhysical_AttachToPhysicalUsingPhysics), "8B 85 40 01 00 00 89 44 24 20 E8", [](soup::Pointer p)
		{
			p = p.add(11).rip();
			CHECK_EXISTING_HOOK(CODENAME(CPhysical_AttachToPhysicalUsingPhysics), "48 8B C4 48 89");
			INIT_HOOK_OPTIONAL(CPhysical_AttachToPhysicalUsingPhysics);
		});
		BATCH_ADD_OPTIONAL(CODENAME(CPickup_AssignCustomArchetype), "FF 90 90 01 00 00 48 8B CB E8 ? ? ? ? 48 8B 83 D0 00 00 00", [](soup::Pointer p)
		{
			p = p.add(10).rip();
			CHECK_EXISTING_HOOK(CODENAME(CPickup_AssignCustomArchetype), "48 8B C4 48 89");
			INIT_HOOK_OPTIONAL_LONGJUMP(CPickup_AssignCustomArchetype);
		});
		/*BATCH_ADD_OPTIONAL(CODENAME(rage_phConstraintMgr_AllocateAndConstruct), "48 8B CB E8 ? ? ? ? 49 89 06", [](soup::Pointer p)
		{
			p = p.add(4).rip();
			CHECK_EXISTING_HOOK(CODENAME(rage_phConstraintMgr_AllocateAndConstruct), "48 89 5C 24 08");
			INIT_HOOK_OPTIONAL(rage_phConstraintMgr_AllocateAndConstruct);
		});*/
		BATCH_ADD_OPTIONAL_HOOK(rage_phConstraintBase_ctor, "48 89 5C 24 08", "48 89 7C 24 10 4C 8B CA 4C 8B C1 48 8D 05");
		BATCH_ADD_OPTIONAL_LONGJUMP_HOOK(CGameScriptHandlerMgr_AddToReservationCount, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 30 48 8B DA 4C 8B F1 BA 00 40 00 00");
		BATCH_ADD_OPTIONAL_HOOK(CDynamicEntity_GetBoneIndexFromBoneTag, "48 89 5C 24 10", "48 89 74 24 18 57 48 83 EC 20 48 8B 01 83 4C 24 30 FF");
		BATCH_ADD_OPTIONAL_HOOK(CBodyRecoilIkSolver_PreIkUpdate, "48 8B C4 48 89", "58 08 48 89 70 10 57 48 81 EC ? 00 00 00 F6 41 78 04");
		BATCH_ADD_OPTIONAL(CODENAME(ARTFeedbackInterfaceGta_onBehaviourFailure), "48 8D 4C 24 20 E8 ? ? ? ? 48 8D 4C 24 20 E8 ? ? ? ? 41 B0 01", [](soup::Pointer p)
		{
			p = p.add(6).rip();
			CHECK_EXISTING_HOOK(CODENAME(ARTFeedbackInterfaceGta_onBehaviourFailure), "48 89 5C 24 08");
			INIT_HOOK_OPTIONAL_LONGJUMP(ARTFeedbackInterfaceGta_onBehaviourFailure);
		});
		BATCH_ADD_OPTIONAL_HOOK(CTaskVehicleMountedWeapon_ComputeSearchTarget, "48 8B C4 48 89", "58 08 48 89 68 10 48 89 70 18 57 48 81 EC A0 00 00 00 48 8B B2");
	}

	std::vector<DetourHook*> ComponentCrashPatch::getHooks()
	{
		return {
			&report_error_wrap_hook,
			&CPopCycle_UpdateCurrZone_hook,
			&unk_cherax_crash_hook,
			&unk_cherax_crash_2_hook,
			&CVehicle_SlideMechanicalPart_hook,
			&CPed_SwitchToRagdollInternal_hook,
			&rage_crSkeleton_GetGlobalMtx_hook,
			&CTorsoIkSolver_ApplyPointTorsoInDirection_hook,
			&CLightEntity_GetWorldMatrix_hook,
			&unk_izuku_crash_hook,
			&rage_grcProgram_SetTextureResourcesUsingVectorDXAPICall_hook,
			&ped_component_draw_thingy_hook,
			&draw_object_hook,
			&CCustomShaderEffectVehicleType_RestoreModelInfoDrawable_hook, 
			&update_ped_hair_texture_hook,
			&CTaskHeliPassengerRappel_do_something_hook,
			&rage_ComponentInfoManager_CreateComponentInfo_hook,
			&unk_related_to_object_despawning_hook,
			&something_related_to_vehicle_parachutes_hook,
			&CObjectFragmentDrawHandler_do_something_hook,
			&CTrain_ValidateLinkedLists_hook,
			&rage_characterClothController_SkinMesh_hook,
			&rage_characterClothController_Update_hook,
			&CPhysical_AttachToPhysicalBasic_hook,
			&CPhysical_AttachToPhysicalUsingPhysics_hook,
			&CPickup_AssignCustomArchetype_hook,
			&rage_phConstraintBase_ctor_hook,
			&CGameScriptHandlerMgr_AddToReservationCount_hook,
			&CDynamicEntity_GetBoneIndexFromBoneTag_hook,
			&CBodyRecoilIkSolver_PreIkUpdate_hook,
			&ARTFeedbackInterfaceGta_onBehaviourFailure_hook,
			&CTaskVehicleMountedWeapon_ComputeSearchTarget_hook,
		};
	}

	// A collection of unpatched flaws in GTA that are networkable.

	// Buffer overruns:
	// - CTrain::SetTrackActive may be called with track index <0 which overwrites data in CPropellerCollisionProcessor which can be exploited by then making a plane have >16 propeller collisions
	//   - via CNetworkTrainReportEvent::Decide
	//   - via CNetworkTrainRequestEvent::Decide (indirect via CTrain::DetermineHostApproval)
	//   - via CNetObjTrain::Update due to CTrainGameStateDataNode with isEngine set to true (as simple as using CREATE_VEHICLE with a train model)
	// - CTaskVehicleGoToPlane, CTaskVehicleLandPlane, & CTaskVehicleFlyDirection assume entity is a CPlane without checking, corrupting data of other entities in pool
	// - CTaskVehicleGoToHelicopter & CTaskVehicleLand assume entity is a CHeli (or derived) without checking, corrupting data of other entities in pool
	// Nullptr dereferences:
	// - CPedVariationStream::SetPaletteTexture assumes that gfx's custom shader effect is not nullptr
	//   Some others we don't bother catching because of generic handlers:
	//   - CNetObjVehicle::SetVehicleControlData assumes the vehicle is a CSubmarineCar and sub handling is not nullptr if m_isSubCar is true
	//   - CTaskExitVehicle::Start_OnEnter assumes ped's helmet component is not nullptr (which does not hold for a_c_rat)
	//   - CTaskParachute::DetachParachuteFromPed assumes parachute object's physics inst is not nullptr
	//   - CTaskGeneralSweep::StateInitial_OnUpdate assumes clip is not nullptr (GetYawMinMaxAndPitch ends up dereferencing it with no check either)
	//   - CTaskParachuteObject::Stream_OnUpdate may use nullptr as rage::fwAnimDirector* to call rage::fwAnimDirector::GetMove
	//   - CTaskJump::StateInit_OnUpdate assumes rage::fwClipSetManager::GetClipSet does not return nullptr (may happen if syncing a_c_rat with this task)
	//   - CTaskClimbLadder::IsMovementBlocked assumes GetBipedCapsuleInfo does not return nullptr (may happen if syncing a_c_rat with this task)
	// Unitialised memory:
	// - CTrainGameStateDataNode may have track index >11 causing train entity to use unitialised track data or even go out of bounds 
	// - rage::phConstraintAttachment constructor does not initialise m_RotationConstraint to nullptr in case translation constraint fails to allocate, which would cause DisableManifolds (which is called in FlagForDestruction) to use the uninitialised m_RotationConstraint as a pointer
	// - rage::phConstraintFixed constructor does not initialise m_TranslationManifold & m_RotationManifold to nullptr in case AllocateManifold fails, causing DisableManifolds (which is called in FlagForDestruction) to use garbage memory as pointers
	// - rage::phConstraintFixedRotation constructor does not initialise m_RotationManifold to nullptr in case AllocateManifold fails, causing DisableManifolds (which is called in FlagForDestruction) to use garbage memory as a pointer
	// - rage::phConstraintPrismatic constructor does not initialise m_Manifolds items to nullptr in case AllocateManifold, causing DisableManifolds (which is called in FlagForDestruction) to use garbage memory as a pointer (also m_RotationConstraint->DisableManifolds will be a nullptr dereference if we get that far)
}
