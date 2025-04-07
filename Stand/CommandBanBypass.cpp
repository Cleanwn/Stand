#if false
#include "CommandBanBypass.hpp"

#include "FiberPool.hpp"
#include "pointers.hpp"
#include "rlRosCredentials.hpp"
#include "ScriptMgr.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"

namespace Stand
{
	static void patch_native_return(uint64_t native_og_hash, bool retval)
	{
		auto p = (uint8_t*)g_script_mgr.entrypoint_map.at(native_og_hash);

		// rip
		++p;
		p = p + *(int32_t*)p + 4;

		// mov rax, qword ptr [rcx]
		*(p + 0) = 0x48;
		*(p + 1) = 0x8b;
		*(p + 2) = 0x01;

		// mov qword ptr [rax], retval
		*(p + 3) = 0x48;
		*(p + 4) = 0xc7;
		*(p + 5) = 0x00;
		*(p + 6) = retval;
		*(p + 7) = 0x00;
		*(p + 8) = 0x00;
		*(p + 9) = 0x00;

		// ret
		*(p + 10) = 0xc3;
	}

	CommandBanBypass::CommandBanBypass(CommandList* parent)
		: CommandAction(parent, LIT("OP Ban Bypass"), {}, LIT("First of all, please only use this on a banned account. Second of all, keep your expectations low, the most you'll get is Creator Mode."))
	{
	}

	static bool running = false;

	void CommandBanBypass::onClick(Click& click)
	{
		if (running || !click.isBasicEdition())
		{
			return;
		}
		running = true;
		ensureScriptThread(click, []
		{
			patch_native_return(0xAF50DA1A3F8B1BA4, true); // NETWORK_CAN_ACCESS_MULTIPLAYER - 48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B DA 48 8B F1 E8 ? ? ? ? 84 C0 75 12 48 85 DB 74 06
			patch_native_return(0xF13FE2A80C05C561, true); // ARE_ONLINE_POLICIES_UP_TO_DATE - 40 53 48 83 EC 20 48 8B 1D ? ? ? ? 48 85 DB 75 25
			patch_native_return(0x72D918C99BCACC54, true); // NETWORK_HAVE_USER_CONTENT_PRIVILEGES - B2 01 B9 FD FF FF FF E8 ? ? ? ? 84 C0 75 07 .add(8).rip()
			patch_native_return(0x9A4CF4F48AD77302, true); // NETWORK_IS_CLOUD_AVAILABLE - 75 EA C6 05 ? ? ? ? 00 48 83 C4 28 - *p.add(4).rip().as<bool*>() = true;
			patch_native_return(0xC0E0D686DDFC6EAE, true); // STAT_GET_LOAD_SAFE_TO_PROGRESS_TO_MP_FROM_SP - 40 53 48 83 EC 40 48 8D 0D ? ? ? ? 83 CA FF E8
			//patch_native_return(0x0B0CC10720653F3B, true); // CLOUD_GET_AVAILABILITY_CHECK_RESULT
			patch_native_return(0xA651443F437B1CE6, true); // STAT_LOAD
			patch_native_return(0xA1750FFAFA181661, false); // STAT_LOAD_PENDING
			patch_native_return(0x7F2C4CDF2E82DF4C, false); // STAT_CLOUD_SLOT_LOAD_FAILED

			FiberPool::queueJob([]
			{
				while (true)
				{
					pointers::ros_credentials->privileges = -1;
					pointers::ros_credentials->revokePrivilege(rage::rlRosPrivilegeId::BANNED);
					pointers::ros_credentials->revokePrivilege(rage::rlRosPrivilegeId::CHEATER);

					// As of b2699

					// drawDebugText(ScriptGlobal(2727812).get<int>()); // TRANSITION_TRACKING, somewhat useful to figure out where maintransition gave up, also see TransitionHelper

					// HAVE_CODE_STATS_LOADED
					ScriptGlobal(1574538).at(0, 1).set<>(TRUE);
					ScriptGlobal(1574538).at(1, 1).set<>(TRUE);
					ScriptGlobal(1574538).at(2, 1).set<>(TRUE);
					ScriptGlobal(1574538).at(3, 1).set<>(TRUE);
					ScriptGlobal(1574538).at(4, 1).set<>(TRUE);

					// RUN_INIT_SHOP_SESSION: Skip loading stage to 4, return true
					ScriptGlobal(2779453).set<>(4);

					/*if (ScriptGlobal(GLOBAL_TRANSITION_STATE).get<int>() == 24) // START_FM_LAUNCH_SCRIPT
					{
						SCRIPT::REQUEST_SCRIPT("freemode");
						if (SCRIPT::HAS_SCRIPT_LOADED("freemode")
							&& SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(ATSTRINGHASH("freemode")) <= 0
							)
						{
							SYSTEM::START_NEW_SCRIPT("freemode", 32750);
						}
					}*/

					// Old stuff for 1.57 LS Tuners
					if constexpr (false)
					{
						*ScriptGlobal(2453009 + 835).as<int*>() = 0;

						*ScriptGlobal(1312392).at(0, 1).as<int*>() = 1;
						*ScriptGlobal(1312392).at(1, 1).as<int*>() = 1;
						*ScriptGlobal(1312392).at(2, 1).as<int*>() = 1;

						*ScriptGlobal(2463514).as<int*>() = 1;

						*ScriptGlobal(2510867 + 1).as<int*>() = 1;
						*ScriptGlobal(2510867 + 4).as<int*>() = 0;

						//*script_global(2453009 + 693).as<int*>() = 1; // finish tutorial mission (not actually helpful as it just breaks)

						//*script_global(1312396).at(1, 1).as<int*>() = 1;

						auto* const tsptr = ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>();
						switch (*tsptr)
						{
						case TRANSITION_STATE_PRE_FM_LAUNCH_SCRIPT:
							if (TransitionHelper::getTimeWastedOnTransitionState() > 4000)
							{
								*tsptr = TRANSITION_STATE_FM_SWOOP_DOWN;
							}
							break;

						case TRANSITION_STATE_PRE_HUD_CHECKS:
							if (TransitionHelper::getTimeWastedOnTransitionState() > 4000)
							{
								*tsptr = TRANSITION_STATE_PREJOINING_FM_SESSION_CHECKS;
							}
							break;

						case TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER:
							if (TransitionHelper::getTimeWastedOnTransitionState() > 4000)
							{
								*tsptr = TRANSITION_STATE_IS_FM_AND_TRANSITION_READY;
							}
							break;
						}
					}

					Script::current()->yield();
				}
			});
		});
	}
}
#endif
