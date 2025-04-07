#include "ComponentGpu.hpp"

#include <soup/os.hpp>

#include "get_current_time_millis.hpp"
#ifdef STAND_DEBUG
#include "Util.hpp"
#endif

#include "ComponentImpl.hpp"

namespace Stand
{
	using rage_grcDevice_IsFencePending_t = bool(__fastcall*)(void* fence);

	static rage_grcDevice_IsFencePending_t rage_grcDevice_IsFencePending{};

	static DetourHook rage_grcDevice_CpuWaitOnFence_hook{ "V5" };

	void rage_grcDevice_CpuWaitOnFence(void* fence)
	{
		auto start = get_current_time_millis();
#ifdef STAND_DEBUG
		uint8_t i = 0;
#endif
		while (rage_grcDevice_IsFencePending(fence))
		{
#ifdef STAND_DEBUG
			if (++i == 0xFF)
			{
				Util::toast("Deadlock in CpuWaitOnFence, pulling the break;s", TOAST_ALL);
				break;
			}
#endif
			soup::os::sleep(1);
		}
		g_comp_gpu.overhead_ms = GET_MILLIS_SINCE(start);
	}

	void ComponentGpu::addPatterns(PatternBatch& batch)
	{
		BATCH_ADD_OPTIONAL("V5", "20 48 8B D9 EB 0D B9 01 00 00 00 E8 ? ? ? ? 48 8B CB 33 D2 E8", [](soup::Pointer p)
		{
			p = p.sub(5);
			CHECK_EXISTING_HOOK("V5", "40 53 48 83 EC");
			INIT_HOOK_OPTIONAL(rage_grcDevice_CpuWaitOnFence);

			p = p.add(27).rip();
			CHECK_EXISTING_HOOK_WONT_HOOK("V6", "48 89 5C 24 10");
			STORE_POINTER(rage_grcDevice_IsFencePending);
		});
	}

	std::vector<DetourHook*> ComponentGpu::getHooks()
	{
		return {
			&rage_grcDevice_CpuWaitOnFence_hook,
		};
	}
}
