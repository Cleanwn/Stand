#include "TickMgr.hpp"

#include "Auth.hpp"
#include "Codename.hpp"
#include "eTaskType.hpp"
#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "gta_input.hpp"
#include "Gui.hpp"
#include "input.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "script_vm_reimpl.hpp"
#include "tbTempEmptyParachute.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"

#include "AbstractEntity.hpp"
#include "AllEntitiesEveryTick.hpp"
#include "AntiAntiCheat.hpp"
#include "AntiEntitySpam.hpp"
#include "AttackerMgr.hpp"
#include "BattlEyeServer.hpp"
#include "BodyguardMgr.hpp"
#include "drawDebugText.hpp"
#include "get_current_time_millis.hpp"
#include "GridToaster.hpp"
#include "GunPunishments.hpp"
#include "is_session.hpp"
#include "Metrics.hpp"
#include "NativeTableHooks.hpp"
#include "NotifyGrid.hpp"
#include "PickupManager.hpp"
#include "Pong.hpp"
#include "regular_event.hpp"
#include "RemoteGamer.hpp"
#include "ScAccount.hpp"
#include "SessionSpoofing.hpp"
#include "SpawnedVehicleMgr.hpp"
#include "StatCache.hpp"
#include "WeaponMgr.hpp"

namespace Stand
{
	bool PerTickToggleable::isActiveThisTick() const noexcept
	{
		return this_tick || every_tick;
	}

	bool PerTickToggleable::onTick() noexcept
	{
		if (this_tick)
		{
			this_tick = false;
			return true;
		}
		return every_tick;
	}

	extern "C"
	{
		// script_vm_helper.asm
		uint64_t script_vm_op_native_tick();

		// overrun_helper.asm
		bool overrun_helper_tick();
	}

	[[nodiscard]] static uint64_t getNativesGameInvokedLastTick()
	{
#if SVM_REIMPL
		if (g_use_script_vm_reimpl)
		{
			auto rax = Metrics::natives_game_invoked_this_tick;
			Metrics::natives_game_invoked_this_tick = 0;
			return rax;
		}
#endif
		return script_vm_op_native_tick();
	}

	void TickMgr::onTick()
	{
		explosion_this_tick = false;

		++Metrics::ticks_this_second;
		Metrics::entity_handles_last_tick = Metrics::entity_handles_this_tick;
		Metrics::entity_handles_this_tick = 0;
		Metrics::natives_game_invoked_last_tick = getNativesGameInvokedLastTick();
		Metrics::natives_stand_invoked_last_tick = Metrics::natives_stand_invoked_this_tick;
		Metrics::natives_stand_invoked_this_tick = 0;

		SOUP_IF_UNLIKELY (overrun_helper_tick())
		{
			Util::onPreventedCrash(Codename("P9"));
		}

		SOUP_IF_LIKELY (*pointers::CLoadingScreens_ms_Context == 0)
		{
			if (g_toaster == &g_grid_toaster)
			{
				g_notify_grid.onTick();
			}

			AbstractEntity::onTick();
			AllEntitiesEveryTick::onTick();

			reScriptTickEvent::trigger();

			if (!is_session_transition_active(*ScriptGlobal(GLOBAL_TRANSITION_STATE).as<TransitionState*>(), false))
			{
				WeaponMgr::onTick();
				GunPunishments::onTick();

				const time_t now = get_current_time_millis();
				PickupManager::onTick(now);
			}

			AntiAntiCheat::onTick();
			AntiEntitySpam::onTick();
			AttackerMgr::onTick();
			BodyguardMgr::onTick();
			NativeTableHooks::onTick();
			Pong::onTick();
			SpawnedVehicleMgr::onTick();
			StatCache::onTick();

			if (BattlEyeServer::isRunningDueToUs())
			{
				BattlEyeServer::tick();
			}
		}

		onTick_drawDebugText();
		ScAccount::onTick();

		if (g_gui.initial_load_done)
		{
			RemoteGamer::onTick(); // before onTickUltimate to give 1 tick window for track/bulk requests to be upgraded
			if (g_auth.license_permissions >= LICPERM_REGULAR)
			{
				SessionSpoofing::onTick();
				if (g_auth.license_permissions == LICPERM_ULTIMATE)
				{
					RemoteGamer::onTickUltimate();
				}
			}
		}

		if (expammo.onTick())
		{
			Vector3 coords;
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(g_player_ped, &coords))
			{
				FIRE::ADD_OWNED_EXPLOSION(g_player_ped, coords.x, coords.y, coords.z, expammo_type, 1, true, false, 0);
			}
		}

		if (super_jump.onTick())
		{
			if (Input::canMovementCommandPerformMovement()
				&& Input::isMenuClosedOrControllerUnused()
				&& Input::isControlJustPressed(INPUT_JUMP)
				&& !is_super_jumping
				&& g_player_ped.isOnGround()
				&& !TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_GET_UP)
				&& PED::GET_VEHICLE_PED_IS_ENTERING(g_player_ped) == NULL_IN_SCRIPTING_LANGUAGE
				&& !g_player_ped.isAiming()
				&& g_player_ped.hasHeadroom()
				)
			{
				doSuperJump();
			}
		}
	}

	void TickMgr::onPostTick()
	{
		AntiAntiCheat::onPostTick();

		rePostTickEvent::trigger();
	}

	void TickMgr::doSuperJump()
	{
		is_super_jumping = true;

		g_tb_temp_empty_parachute.enable();

		//int i;
		//TASK::OPEN_SEQUENCE_TASK(&i);
		TASK::TASK_JUMP(g_player_ped, TRUE, TRUE, FALSE);
		//TASK::CLOSE_SEQUENCE_TASK(i);
		//TASK::TASK_PERFORM_SEQUENCE(g_player_ped, i);
		//TASK::CLEAR_SEQUENCE_TASK(&i);

		FiberPool::queueJob([=]
		{
			bool abort = false;

			time_t deadline = get_current_time_millis() + 5000;
			while (!TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_JUMP))
			{
				if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_VAULT))
				{
					abort = true;
				}
				if (IS_DEADLINE_REACHED(deadline))
				{
					abort = true;
					break;
				}
				Script::current()->yield();
			}

			if (abort)
			{
				if (!g_player_veh.isValid())
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
				}
			}
			else
			{
				is_super_jumping_for_user = true;
				while (g_player_ped.isOnGround())
				{
					Script::current()->yield();
				}

				// Wait for jump to complete
				while (!g_player_ped.isOnGround()
					&& TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_JUMP)
					)
				{
					Script::current()->yield();
				}
				is_super_jumping_for_user = false;

				// Wait for possible ragdoll tasks to start
				Script::current()->yield(500);

				// Ragdolling now?
				if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_NM_CONTROL))
				{
					// Ensure the ped won't re-attempt the jump
					do
					{
						Script::current()->yield();
					} while (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_NM_CONTROL));

					if (!g_player_veh.isValid())
					{
						TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
					}
				}
			}
			is_super_jumping = false;
			g_tb_temp_empty_parachute.disable();
		});
	}
}
