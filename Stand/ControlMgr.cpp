#include "ControlMgr.hpp"

#include <Windows.h>

#include "atStringHash.hpp"
#include "gta_input.hpp"
#include "NativeTableHooks.hpp"
#include "script_thread.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	bool ControlMgr::needsHook() noexcept
	{
		return gun_freedom
			|| move_freedom
			|| rc_freecam
			|| mobile_radio
			;
	}

	void ControlMgr::update() noexcept
	{
		if (needsHook())
		{
			if (!has_hook)
			{
				has_hook = true;

				// DISABLE_CONTROL_ACTION(int padIndex, int control, BOOL disable)
				NativeTableHooks::createNativeEntrypointOverride(0xFE99B66D079CF6BC, [](rage::scrNativeCallContext& ctx)
				{
					if (ctx.getArg<BOOL>(2))
					{
						if (gun_freedom)
						{
							switch (ctx.getArg<int>(1))
							{
							case INPUT_NEXT_CAMERA:
							case INPUT_WEAPON_WHEEL_UD:
							case INPUT_WEAPON_WHEEL_LR:
							case INPUT_WEAPON_WHEEL_NEXT:
							case INPUT_WEAPON_WHEEL_PREV:
							case INPUT_SELECT_NEXT_WEAPON:
							case INPUT_SELECT_PREV_WEAPON:
							case 19:
							case 23:
							case 24:
							case 25:
							case 27:
							case 37:
							case INPUT_COVER:
							case 45:
							case 47:
							case 48:
							case 49:
							case 50:
							case 51:
							case 68:
							case 69:
							case 70:
							case 86:
							case 92:
							case 99:
							case 100:
							case 114:
							case 115:
							case 140:
							case 141:
							case 142:
							case 143:
							case 157:
							case 158:
							case 159:
							case 160:
							case 161:
							case 162:
							case 163:
							case 164:
							case 165:
							case 257:
								return;
							}
						}
						if (move_freedom)
						{
							switch (ctx.getArg<int>(1))
							{
							case INPUT_SPRINT:
							case INPUT_JUMP:
							case INPUT_DUCK:
							case INPUT_VEH_JUMP:
								return;
							}
						}
						if (mobile_radio)
						{
							switch (ctx.getArg<int>(1))
							{
							case INPUT_VEH_RADIO_WHEEL:
							case INPUT_RADIO_WHEEL_UD:
							case INPUT_RADIO_WHEEL_LR:
								return;
							}
						}
					}
					if (rc_freecam)
					{
						if (rage::scrThread::get()->m_context.m_script_hash == ATSTRINGHASH("am_mp_rc_vehicle"))
						{
							return;
						}
					}
					NativeTableHooks::og(0xFE99B66D079CF6BC)(ctx);
				});
			}
		}
		else
		{
			if (has_hook)
			{
				has_hook = false;

				NativeTableHooks::removeNativeEntrypointOverride(0xFE99B66D079CF6BC);
			}
		}
	}
}

