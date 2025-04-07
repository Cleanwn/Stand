#include "ComponentSpoofPos.hpp"

#include "AbstractEntity.hpp"
#include "is_session.hpp"

#include "ComponentImpl.hpp"

namespace Stand
{
	static DetourHook CNetObjEntity_GetPosition_hook{ "QH" };

	//static v3 __fastcall CNetObjEntity_GetPosition(CNetObjEntity* _this)
	static v3* __fastcall CNetObjEntity_GetPosition(CNetObjEntity* _this, v3& result)
	{
		__try
		{
			if (_this == g_player_ent.getNetObject<CNetObjEntity>())
			{
				if (g_comp_spoofpos.spoof_position)
				{
					if (is_session_started()) // Fix for sometimes getting teleported to spoofed position when joining session.
					{
						result = g_comp_spoofpos.spoofed_position;
						return &result;
					}
				}
			}
		}
		__EXCEPTIONAL()
		{
		}
		return COMP_OG(CNetObjEntity_GetPosition)(_this, result);
	}

	void ComponentSpoofPos::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD_OPTIONAL_HOOK(CNetObjEntity_GetPosition, "40 53 48 83 EC", "20 48 8B 01 48 8B DA FF 90 C0 00 00 00 33 C9 48 89 0B");
	}

	std::vector<DetourHook*> ComponentSpoofPos::getHooks()
	{
		return {
			&CNetObjEntity_GetPosition_hook
		};
	}
}
