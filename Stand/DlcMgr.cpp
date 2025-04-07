#include "DlcMgr.hpp"

#include <Windows.h>

#include "NativeTableHooks.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	bool DlcMgr::isHookNeeded()
	{
		return dev_flag
			|| preorder_bonus_override != 0
			|| admin_dlc
			;
	}

	void DlcMgr::update()
	{
		if (isHookNeeded())
		{
			if (!has_hook)
			{
				has_hook = true;
				NativeTableHooks::createNativeEntrypointOverride(0x812595A0644CE1DE, [](rage::scrNativeCallContext& ctx)
				{
					switch (ctx.getArg<Hash>(0))
					{
					case -65181770: // ATSTRINGHASH("spPreorder") - Pre-order bonus
						if (preorder_bonus_override != 0)
						{
							return ctx.setReturnValue<BOOL>(preorder_bonus_override - 1);
						}
						break;

					case 2532323046: // ATSTRINGHASH("XX_I$RAWKST4H_D3V_XX") - Admin DLC
						return ctx.setReturnValue<BOOL>(admin_dlc);
					}
					return NativeTableHooks::og(0x812595A0644CE1DE)(ctx);
				});
			}
		}
		else
		{
			if (has_hook)
			{
				has_hook = false;
				NativeTableHooks::removeNativeEntrypointOverride(0x812595A0644CE1DE);
			}
		}
	}
}
