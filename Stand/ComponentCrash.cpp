#include "ComponentCrash.hpp"

#include "gta_net_object_mgr.hpp"

#include "ComponentImpl.hpp"

namespace Stand
{
	static DetourHook CNetObjTrain_GetTrainGameState_hook{ "F1" };

	static void __fastcall CNetObjTrain_GetTrainGameState(uintptr_t a1, CTrainGameStateDataNode* node)
	{
		__try
		{
			COMP_OG(CNetObjTrain_GetTrainGameState)(a1, node);
			if (g_comp_crash.car1.isValid()
				&& g_comp_crash.car1.getNetObject()
				&& g_comp_crash.car2.getNetObject()
				&& g_comp_crash.car3.getNetObject()
				)
			{
				auto train = *reinterpret_cast<CTrain**>(a1 - 0x8D0);
				// Some menus only seem to check backwards linkage (calling the CTrain::GetLinkedToBackward function to do so)
				// but even if we only recurse with forward linkage, that still seems to block this, because this game makes no sense.
				if (train == reinterpret_cast<CTrain*>(g_comp_crash.car1.getPointer()))
				{
					node->m_TrackID = 69;
					node->m_LinkedToBackwardID = g_comp_crash.car2.getNetObject()->object_id;
					node->m_LinkedToForwardID = g_comp_crash.car3.getNetObject()->object_id;
				}
				else if (train == reinterpret_cast<CTrain*>(g_comp_crash.car2.getPointer()))
				{
					node->m_IsEngine = true;
					node->m_TrackID = -31;
					node->m_TrainConfigIndex = 69;
					node->m_CarriageConfigIndex = 69;
					node->m_LinkedToBackwardID = g_comp_crash.car3.getNetObject()->object_id;
					node->m_LinkedToForwardID = g_comp_crash.car1.getNetObject()->object_id;
				}
				else if (train == reinterpret_cast<CTrain*>(g_comp_crash.car3.getPointer()))
				{
					node->m_IsEngine = true;
					node->m_TrackID = 26;
					node->m_TrainConfigIndex = 69;
					node->m_CarriageConfigIndex = 69;
					node->m_LinkedToBackwardID = g_comp_crash.car1.getNetObject()->object_id;
					node->m_LinkedToForwardID = g_comp_crash.car2.getNetObject()->object_id;
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	void ComponentCrash::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD_OPTIONAL_HOOK(CNetObjTrain_GetTrainGameState, "48 89 5C 24 08", "48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B B9 30 F7 FF FF");
	}

	std::vector<DetourHook*> ComponentCrash::getHooks()
	{
		return {
			&CNetObjTrain_GetTrainGameState_hook,
		};
	}
}
