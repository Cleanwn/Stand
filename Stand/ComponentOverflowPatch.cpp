#include "ComponentOverflowPatch.hpp"

#include "AbstractEntity.hpp"
#include "atFixedArray.hpp"
#include "CPedDrawHandler.hpp"
#include "CPedModelInfo.hpp"
#include "DoVehicleLightsAsync.hpp"
#include "fwAltSkeletonExtension.hpp"
#include "fwPool.hpp"
#include "fwStepAllocator.hpp"
#include "gta_net_object_mgr.hpp"
#include "gta_ped.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "Matrix34.hpp"
#include "pointers.hpp"
#include "sysDependency.hpp"
#include "Util.hpp"

#include "ComponentImpl.hpp"

namespace Stand
{
	using AltSkeletonExtensionArray = rage::atFixedArray<rage::fwAltSkeletonExtension, 32>;

	using CVehicle_DoVehicleLightsAsync_t = void(__fastcall*)(CVehicle* _this, const rage::Matrix34* matVehicle, unsigned int nLightFlags, const rage::Vec4V& ambientVolumeParams, const rage::fwInteriorLocation interiorLoc, CoronaAndLightList& coronaAndLightList);

	static CTrainTrack* gTrainTracks;
	static uint32_t* rage_fwTimer_sm_gameTime_m_Time;
	static AltSkeletonExtensionArray* alt_skeleton_extensions_array;
	static unsigned int* alt_skeleton_extension_id;
	static CVehicle_DoVehicleLightsAsync_t CVehicle_DoVehicleLightsAsync;
	static rage::fwBasePool** CVehicleStreamRequestGfx_pool;

	// Y0 is unused
	static DetourHook CPedVariationStream_SetVariation_hook{ "Y1" };
	static DetourHook CTrain_SetTrackActive_hook{ "Y2" };
	static DetourHook rage_fwAltSkeletonExtension_GetOrAddExtension_hook{ "Y3" };
	static DetourHook DoVehicleLightsAsync_RunFromDependency_hook{ "Y4" };
	static DetourHook rage_netObjectIDMgr_GetNewObjectId_hook{ "Y5" };
	static DetourHook CVehicleVariationInstance_RequestVehicleModFiles_hook{ "Y6" };

	static bool CPedVariationStream_SetVariation(CPed* ped, CPedPropData* pedPropData, CPedVariationData* pedVarData, ePedVarComp_t slotId, uint32_t drawblId, uint32_t drawablAltId, uint32_t texId, uint32_t paletteId, int32_t streamFlags, bool force)
	{
		__try
		{
			if (texId != 0 && texId != -1)
			{
				SOUP_IF_UNLIKELY (texId >= static_cast<CPedModelInfo*>(ped->archetype)->GetVarInfo()->GetMaxNumTex(slotId, drawblId))
				{
					SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
					{
						// The texId going out of bounds causes a buffer overrun, which can result in all sorts of nasty, indirect crashes. See scripts/invalid-tex.lua.
						Util::onPreventedBufferOverrun(LOC("BU_TEX"));
						texId = 0;
					}
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		return COMP_OG(CPedVariationStream_SetVariation)(ped, pedPropData, pedVarData, slotId, drawblId, drawablAltId, texId, paletteId, streamFlags, force);
	}

	// OG does not have >= 0 check. It is easily possible that our local trains have this issue.
	// And legit players may sync shit like this as well via CNetworkTrainReportEvent, so this is probably the best approach for now.
	static void __fastcall CTrain_SetTrackActive(int trackindex, bool trainactive)
	{
		SOUP_IF_UNLIKELY (trackindex < 0 || trackindex >= 27)
		{
			SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
			{
#ifdef STAND_DEBUG
				Util::onPreventedBufferOverrun(CODENAME(CTrain_SetTrackActive));
#endif
				return;
			}
		}
		gTrainTracks[trackindex].m_bTrainActive = trainactive;
		gTrainTracks[trackindex].m_iLastTrainReportingTimeMS = trainactive ? *rage_fwTimer_sm_gameTime_m_Time : 0;
	}

	// rage::fwAltSkeletonExtension fixed array overflow.
	// Can happen when someone uses rapid fire and remove sticky bomb limit to throw stickies on our back and we enter first person.
	static rage::fwAltSkeletonExtension* __fastcall rage_fwAltSkeletonExtension_GetOrAddExtension(rage::fwEntity& entity)
	{
		if (auto ext = entity.extensions.Get(*alt_skeleton_extension_id))
		{
			return static_cast<rage::fwAltSkeletonExtension*>(ext);
		}
		SOUP_IF_UNLIKELY (!alt_skeleton_extensions_array->canAppend())
		{
			SOUP_IF_LIKELY (g_gui.doesRootStateAllowCrashPatches())
			{
				Util::onPreventedCrash(CODENAME(rage_fwAltSkeletonExtension_GetOrAddExtension));
				return nullptr;
			}
		}
		return COMP_OG(rage_fwAltSkeletonExtension_GetOrAddExtension)(entity);
	}

	static bool __fastcall DoVehicleLightsAsync_RunFromDependency(const rage::sysDependency& dependency)
	{
		SOUP_IF_UNLIKELY (!g_gui.doesRootStateAllowCrashPatches())
		{
			return COMP_OG(DoVehicleLightsAsync_RunFromDependency)(dependency);
		}

		void* scratchMemory = dependency.m_Params[0].m_AsPtr;
		rage::fwStepAllocator scratchAllocator;
		scratchAllocator.Init(scratchMemory, DO_VEHICLE_LIGHTS_ASYNC_SCRATCH_SIZE);
		Corona_t* pLocalCoronas = scratchAllocator.Alloc<Corona_t>(MAX_NUM_CORONAS_PER_JOB, 16);
		CLightSource* pLocalLightSources = scratchAllocator.Alloc<CLightSource>(MAX_NUM_LIGHTS_PER_JOB, 16);

		DoVehicleLightsAsyncData* pData = static_cast<DoVehicleLightsAsyncData*>(dependency.m_Params[1].m_AsPtr);
		uint32_t* pDependencyRunning = static_cast<uint32_t*>(dependency.m_Params[2].m_AsPtr);
		int firstVehicleIndex = dependency.m_Params[3].m_AsInt;
		int lastVehicleIndex = dependency.m_Params[4].m_AsInt;

		__try
		{
			SOUP_ASSERT(lastVehicleIndex < 300 /*MAX_VEHICLES_FOR_ASYNC_LIGHTS*/);

			CVehicle** aVehicles = pData->m_paVehicles;
			uint32_t* aLightFlags = pData->m_paLightFlags;
			rage::Vec4V* aAmbientVolumeParams = pData->m_paAmbientVolumeParams;
			rage::fwInteriorLocation* aInteriorLocs = pData->m_paInteriorLocs;
			Corona_t* volatile* pCoronasCurrPtr = pData->m_pCoronasCurrPtr;
			CLightSource* volatile* pLightsCurrPtr = pData->m_pLightSourcesCurrPtr;

			CoronaAndLightList coronaAndLightList;
			coronaAndLightList.Init(pLocalCoronas, pLocalLightSources, MAX_NUM_CORONAS_PER_JOB, MAX_NUM_LIGHTS_PER_JOB, pData->m_timeCycleSpriteSize, pData->m_timeCycleSpriteBrightness, pData->m_timecycleVehicleIntensityScale);

			for (int i = firstVehicleIndex; i <= lastVehicleIndex; i++)
			{
				CVehicle* pVehicle = aVehicles[i];
				rage::Matrix34 mat(pVehicle->GetMatrix());
				CVehicle_DoVehicleLightsAsync(pVehicle, &mat, aLightFlags[i], aAmbientVolumeParams[i], aInteriorLocs[i], coronaAndLightList);
			}

			uint32_t localCoronaCount = coronaAndLightList.GetCoronaCount();
			uint32_t localLightCount = coronaAndLightList.GetLightCount();
			if (localCoronaCount > 0)
			{
				const uintptr_t	coronasDestPtr = _InterlockedExchangeAdd(reinterpret_cast<uintptr_t volatile*>(pCoronasCurrPtr), ptrdiff_t(sizeof(Corona_t) * localCoronaCount));

				SOUP_IF_LIKELY ((Corona_t*)coronasDestPtr >= pData->m_CoronasStartingPtr && ((Corona_t*)coronasDestPtr + localCoronaCount) < (pData->m_CoronasStartingPtr + pData->m_MaxCoronas))
				{
					memcpy(reinterpret_cast<Corona_t*>(coronasDestPtr), pLocalCoronas, sizeof(Corona_t) * localCoronaCount);
				}
				else
				{
					InterlockedExchangeSubtract(reinterpret_cast<uintptr_t volatile*>(pCoronasCurrPtr), ptrdiff_t(sizeof(Corona_t) * localCoronaCount));
					Util::onPreventedBufferOverrun(LOC("BU_VEHLIT"));
				}
			}
			if (localLightCount > 0)
			{
				const uintptr_t	lightsDestPtr = _InterlockedExchangeAdd(reinterpret_cast<uintptr_t volatile*>(pLightsCurrPtr), ptrdiff_t(sizeof(CLightSource) * localLightCount));

				SOUP_IF_LIKELY ((CLightSource*)lightsDestPtr >= pData->m_LightSourcesStartingPtr && ((CLightSource*)lightsDestPtr + localLightCount) < (pData->m_LightSourcesStartingPtr + pData->m_MaxLightSources))
				{
					memcpy(reinterpret_cast<CLightSource*>(lightsDestPtr), pLocalLightSources, sizeof(CLightSource) * localLightCount);
				}
				else
				{
					InterlockedExchangeSubtract(reinterpret_cast<uintptr_t volatile*>(pLightsCurrPtr), ptrdiff_t(sizeof(CLightSource) * localLightCount));
					Util::onPreventedBufferOverrun(LOC("BU_VEHLIT"));
				}
			}
		}
		__EXCEPTIONAL()
		{
		}

		_InterlockedDecrement(pDependencyRunning);

		return true;
	}

	static uint16_t __fastcall rage_netObjectIDMgr_GetNewObjectId(void* _this)
	{
		uint16_t id = COMP_OG(rage_netObjectIDMgr_GetNewObjectId)(_this);
		SOUP_IF_UNLIKELY ((*pointers::network_object_mgr)->find_object_by_id(id, true))
		{
			Exceptional::report(soup::ObfusString("Game attempted to reuse an object id"));
			do
			{
				++id;
			} while ((*pointers::network_object_mgr)->find_object_by_id(id, true));
		}
		return id;
	}

	static void __fastcall CVehicleVariationInstance_RequestVehicleModFiles(CVehicle* veh, int streamFlags)
	{
		SOUP_IF_UNLIKELY ((*CVehicleStreamRequestGfx_pool)->GetNoOfFreeSpaces() == 0)
		{
			if (g_gui.doesRootStateAllowCrashPatches())
			{
				//Util::onPreventedCrash(CODENAME(CVehicleVariationInstance_RequestVehicleModFiles), g_hooking.veh_appearance_sync_blamer.getBlame());
				if (auto p = g_hooking.veh_appearance_sync_blamer.getBlame(); p != MAX_PLAYERS)
				{
					AbstractPlayer(p).getAndApplyReactionsIn(FlowEvent::SE_CRASH, CODENAME(CVehicleVariationInstance_RequestVehicleModFiles).toString());
				}
				else
				{
					Util::onPreventedCrash(CODENAME(CVehicleVariationInstance_RequestVehicleModFiles));
				}
				return;
			}
		}
		COMP_OG(CVehicleVariationInstance_RequestVehicleModFiles)(veh, streamFlags);
	}

	void ComponentOverflowPatch::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD_OPTIONAL_HOOK(CPedVariationStream_SetVariation, "48 8B C4 44 89", "48 20 4C 89 40 18 48 89 50 10 48 89 48 08 55 53 56 57 48 81 EC D8 00 00 00");
		BATCH_ADD_OPTIONAL(CODENAME(CTrain_SetTrackActive), "83 F9 1B 7D 25 4C 8D 05 ? ? ? ? 48 63 C9 48 69 C9 50 02 00 00 42 88 54 01 40 F6 DA 1B C0 23 05", [](soup::Pointer p)
		{
			INIT_HOOK_OPTIONAL(CTrain_SetTrackActive);
			gTrainTracks = p.add(8).rip().as<CTrainTrack*>();
			rage_fwTimer_sm_gameTime_m_Time = p.add(33).rip().as<uint32_t*>();
		});
		BATCH_ADD_OPTIONAL(CODENAME(rage_fwAltSkeletonExtension_GetOrAddExtension), "48 8B CB E8 ? ? ? ? 48 89 44 24 60", [](soup::Pointer p)
		{
			p = p.add(4).rip();

			CHECK_EXISTING_HOOK(CODENAME(rage_fwAltSkeletonExtension_GetOrAddExtension), "48 89 5C 24 08");
			INIT_HOOK_OPTIONAL(rage_fwAltSkeletonExtension_GetOrAddExtension);

			p = p.add(15);
			alt_skeleton_extension_id = p.add(2).rip().as<decltype(alt_skeleton_extension_id)>();
			alt_skeleton_extensions_array = p.add(29).rip().sub(offsetof(AltSkeletonExtensionArray, m_Count)).as<AltSkeletonExtensionArray*>();
		});
		BATCH_ADD_OPTIONAL(CODENAME(DoVehicleLightsAsync_RunFromDependency), "4C 8B 01 BA 80 B1 00 00", [](soup::Pointer p)
		{
			p = p.sub(25);
			CHECK_EXISTING_HOOK(CODENAME(DoVehicleLightsAsync_RunFromDependency), "40 55 53 56 57");
			INIT_HOOK_OPTIONAL_LONGJUMP(DoVehicleLightsAsync_RunFromDependency);

			p = p.add(519);
			CHECK_EXISTING_HOOK(CODENAME(DoVehicleLightsAsync_RunFromDependency), "E8");
			CVehicle_DoVehicleLightsAsync = p.add(1).rip().as<CVehicle_DoVehicleLightsAsync_t>();
		});
		BATCH_ADD_OPTIONAL(CODENAME(rage_netObjectIDMgr_GetNewObjectId), "8B 81 C0 3E 00 00", [](soup::Pointer p)
		{
			p = p.sub(6);
			CHECK_EXISTING_HOOK(CODENAME(rage_netObjectIDMgr_GetNewObjectId), "FF 81 C0 3E 00 00");
			INIT_HOOK_OPTIONAL(rage_netObjectIDMgr_GetNewObjectId);
		});
		BATCH_ADD_OPTIONAL(CODENAME(CVehicleVariationInstance_RequestVehicleModFiles), "8B 15 ? ? ? ? 4C 8B F1 48 83 C1 10 E8 ? ? ? ? 48 8B 0D", [](soup::Pointer p)
		{
			CVehicleStreamRequestGfx_pool = p.add(21).rip().as<rage::fwBasePool**>();

			p = p.sub(0x00007FF749C868CB - 0x00007FF749C868AC);
			CHECK_EXISTING_HOOK(CODENAME(CVehicleVariationInstance_RequestVehicleModFiles), "48 89 5C 24 10");
			INIT_HOOK_OPTIONAL(CVehicleVariationInstance_RequestVehicleModFiles);
		});
	}

	std::vector<DetourHook*> ComponentOverflowPatch::getHooks()
	{
		return {
			&CPedVariationStream_SetVariation_hook,
			&CTrain_SetTrackActive_hook,
			&rage_fwAltSkeletonExtension_GetOrAddExtension_hook,
			&DoVehicleLightsAsync_RunFromDependency_hook,
			&rage_netObjectIDMgr_GetNewObjectId_hook,
			&CVehicleVariationInstance_RequestVehicleModFiles_hook,
		};
	}
}
