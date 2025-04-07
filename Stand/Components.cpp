#include "Components.hpp"

#include "ComponentCrash.hpp"
#include "ComponentCrashPatch.hpp"
#include "ComponentDrawPatch.hpp"
#include "ComponentGpu.hpp"
#include "ComponentNetcode.hpp"
#include "ComponentOverflowPatch.hpp"
#include "ComponentSavedata.hpp"
#include "ComponentSpoofPos.hpp"

namespace Stand
{
	static Component* all_components[] = {
		&g_comp_crash,
		&g_comp_crashpatch,
		&g_comp_drawpatch,
		&g_comp_gpu,
		&g_comp_netcode,
		&g_comp_overflowpatch,
		&g_comp_savedata,
		&g_comp_spoofpos,
	};

	void Components::addPatterns(PatternBatch& batch)
	{
		for (auto& comp : all_components)
		{
			comp->addPatterns(batch);
		}
	}

	void Components::collectHooks(std::vector<DetourHook*>& hooks)
	{
		for (auto& comp : all_components)
		{
			for (auto& hook : comp->getHooks())
			{
				hooks.emplace_back(hook);
			}
		}
	}
}
