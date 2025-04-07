#include "CommandGunFreedom.hpp"

#include "AbstractEntity.hpp"
#include "ControlMgr.hpp"
#include "hud_component.hpp"
#include "NativeTableHooks.hpp"
#include "ScriptGlobal.hpp"
#include "scrNativeCallContext.hpp"

namespace Stand
{
	CommandGunFreedom::CommandGunFreedom(CommandList* const parent)
		: CommandToggle(parent, LOC("TTLFRDM"), CMDNAMES_OBF("gunfreedom", "weaponfreedom"), LOC("TTLFRDM_H"))
	{
	}

	void CommandGunFreedom::onEnable(Click& click)
	{
		ensureScriptThread(click, []
		{
			ControlMgr::gun_freedom = true;
			ControlMgr::update();

			ScriptGlobal(GLOBAL_KICK_OUT_OF_NIGHTCLUB_WHEN_ARMED).set<int>(0);

			// SET_CURRENT_PED_WEAPON(Ped ped, Hash weaponHash, BOOL bForceInHand)
			NativeTableHooks::createNativeEntrypointOverride(0xADF692B254977C0C, [](rage::scrNativeCallContext& ctx)
			{
				if (ctx.getArg<Hash>(1) != ATSTRINGHASH("WEAPON_UNARMED")
					|| !g_player_ped.isHandleEquals(ctx.getArg<Ped>(0))
					)
				{
					NativeTableHooks::og(0xADF692B254977C0C)(ctx);
				}
			});

			// GIVE_WEAPON_TO_PED(Ped ped, Hash weaponHash, int ammoCount, BOOL isHidden, BOOL bForceInHand)
			NativeTableHooks::createNativeEntrypointOverride(0xBF0FD6E56C964FCB, [](rage::scrNativeCallContext& ctx)
			{
				ScriptGlobal(GLOBAL_KICK_OUT_OF_NIGHTCLUB_WHEN_ARMED).set<int>(0);

				if (ctx.getArg<Hash>(1) != ATSTRINGHASH("WEAPON_UNARMED")
					|| !g_player_ped.isHandleEquals(ctx.getArg<Ped>(0))
					)
				{
					NativeTableHooks::og(0xBF0FD6E56C964FCB)(ctx);
				}
			});

			// HIDE_HUD_COMPONENT_THIS_FRAME(int id)
			NativeTableHooks::createNativeEntrypointOverride(0x6806C51AD12B83B8, [](rage::scrNativeCallContext& ctx)
			{
				switch (ctx.getArg<int>(0))
				{
				case HUD_WEAPON_WHEEL:
				case HUD_WEAPON_WHEEL_STATS:
					return;
				}
				NativeTableHooks::og(0x6806C51AD12B83B8)(ctx);
			});

			// _HUD_WEAPON_WHEEL_IGNORE_SELECTION
			NativeTableHooks::createNativeEntrypointOverride(0x0AFC4AF510774B47, &nullsub<rage::scrNativeCallContext&>);
		});
	}

	void CommandGunFreedom::onDisable(Click& click)
	{
		ensureScriptThread(click, []
		{
			ControlMgr::gun_freedom = false;
			ControlMgr::update();

			NativeTableHooks::removeNativeEntrypointOverride(0xADF692B254977C0C);
			NativeTableHooks::removeNativeEntrypointOverride(0xBF0FD6E56C964FCB);
			NativeTableHooks::removeNativeEntrypointOverride(0x6806C51AD12B83B8);
			NativeTableHooks::removeNativeEntrypointOverride(0x0AFC4AF510774B47);

			ScriptGlobal(GLOBAL_KICK_OUT_OF_NIGHTCLUB_WHEN_ARMED).set<int>(1);
		});
	}
}
