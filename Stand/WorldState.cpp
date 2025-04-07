#include "WorldState.hpp"

#include "natives.hpp"
#include "ScriptGlobal.hpp"

namespace Stand
{
	BOOL WorldState::getOnline()
	{
		return ScriptGlobal(GLOBAL_MP_WORLD_STATE).get<BOOL>();
	}

	void WorldState::setOnline(BOOL toggle)
	{
		if (getOnline() != toggle)
		{
			ScriptGlobal(GLOBAL_MP_WORLD_STATE).set<BOOL>(toggle);
			if (toggle)
			{
				DLC::ON_ENTER_MP();
			}
			else
			{
				DLC::ON_ENTER_SP();
			}
		}
	}
}
