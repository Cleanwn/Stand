#include "CommandMoveFreedom.hpp"

#include "AbstractEntity.hpp"
#include "ControlMgr.hpp"
#include "ePedConfigFlags.hpp"
#include "ePedResetFlags.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "regular_event.hpp"

namespace Stand
{
	static void onPostTick()
	{
		PED::SET_PED_MAX_MOVE_BLEND_RATIO(g_player_ped, 3.0f);

		PED::SET_PED_RESET_FLAG(g_player_ped, CPED_RESET_FLAG_DisablePlayerJumping, FALSE);
		PED::SET_PED_RESET_FLAG(g_player_ped, CPED_RESET_FLAG_DisablePlayerVaulting, FALSE);
		//PED::SET_PED_RESET_FLAG(g_player_ped, CPED_RESET_FLAG_NotAllowedToChangeCrouchState, FALSE);
		PED::SET_PED_RESET_FLAG(g_player_ped, CPED_RESET_FLAG_DisablePlayerAutoVaulting, FALSE);
		PED::SET_PED_RESET_FLAG(g_player_ped, CPED_RESET_FLAG_UseInteriorCapsuleSettings, FALSE);

		//PED::SET_PED_CONFIG_FLAG(g_player_ped, CPED_CONFIG_FLAG_NotAllowedToCrouch, FALSE);
		PED::SET_PED_CONFIG_FLAG(g_player_ped, CPED_CONFIG_FLAG_IgnoreInteriorCheckForSprinting, TRUE);
	}

	CommandMoveFreedom::CommandMoveFreedom(CommandList* parent)
		: CommandToggle(parent, LOC("TTLFRDM"), CMDNAMES_OBF("movefreedom", "movementfreedom"), LOC("TTLFRDM_MOV_H"))
	{
	}

	void CommandMoveFreedom::onEnable(Click& click)
	{
		rePostTickEvent::registerHandler(&onPostTick);
		ensureScriptThread(click, []
		{
			ControlMgr::move_freedom = true;
			ControlMgr::update();

			// SET_PED_STEALTH_MOVEMENT
			NativeTableHooks::createNativeEntrypointOverride(0x88CBB5CEB96B7BD2, [](rage::scrNativeCallContext& ctx)
			{
				if (ctx.getArg<BOOL>(1) == FALSE)
				{
					return;
				}
				NativeTableHooks::og(0x88CBB5CEB96B7BD2)(ctx);
			});
		});
	}

	void CommandMoveFreedom::onDisable(Click& click)
	{
		rePostTickEvent::unregisterHandler(&onPostTick);

		ensureScriptThread(click, []
		{
			ControlMgr::move_freedom = false;
			ControlMgr::update();

			PED::SET_PED_CONFIG_FLAG(g_player_ped, CPED_CONFIG_FLAG_IgnoreInteriorCheckForSprinting, FALSE);

			NativeTableHooks::removeNativeEntrypointOverride(0x88CBB5CEB96B7BD2);
		});
	}
}
