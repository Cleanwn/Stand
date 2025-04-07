#include "CommandSeamless.hpp"

#include "Auth.hpp"
#include "CommandListSelect.hpp"
#include "eTaskType.hpp"
#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "pointers.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"
#include "Script.hpp"
#include "script_thread.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "tbScreenshotMode.hpp"
#include "tbStareAtSky.hpp"
#include "TranscendentVehicle.hpp"
#include "TransitionHelper.hpp"
#include "TransitionState.hpp"
#include "Util.hpp"
#include "WorldState.hpp"

namespace Stand
{
	CommandSeamless::CommandSeamless(CommandList* const parent)
		: CommandToggle(parent, LOC("SSS"), { CMDNAME("seamless") })
	{
	}

	static TransitionState prev_ts = TRANSITION_STATE_EMPTY;
	static bool prev_transition_active = false;
	static BOOL online_world_state_desired;
	static BOOL online_world_state_reality;
	static std::wstring centered_text{};
	static time_t warning_message_since = 0;

	bool CommandSeamless::canRun()
	{
		return *pointers::CLoadingScreens_ms_Context == 0
			&& g_auth.license_permissions != LICPERM_FREE
			;
	}

	void CommandSeamless::onPreTick()
	{
		if (!canRun())
		{
			return;
		}
		const auto ts = ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>();
		if (is_session_transition_active(ts))
		{
			// Save player's vehicle
			if (g_player_veh.isValid())
			{
				TranscendentVehicle::save(g_player_veh);

				// Save it in the other sense as well (for aircraft)
				TASK::CLEAR_VEHICLE_CRASH_TASK(g_player_veh);
			}

			// Maintain ground camera
			if (*ScriptGlobal(GLOBAL_TRANS_SWITCH).as<int*>() == 0)
			{
				*ScriptGlobal(GLOBAL_TRANS_SWITCH).as<int*>() = 1;
			}
			else if (*ScriptGlobal(GLOBAL_TRANS_SWITCH).as<int*>() == 1)
			{
				*ScriptGlobal(GLOBAL_TRANS_SWITCH).as<int*>() = 2;
				if (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
				{
					STREAMING::SWITCH_TO_MULTI_SECONDPART(g_player_ped);
				}
			}

			// Unstuck if needed
			if (ts == TRANSITION_STATE_LOOK_FOR_FRESH_JOIN_FM)
			{
				if (TransitionHelper::getTimeWastedOnTransitionState() > 1000)
				{
					ScriptGlobal(GLOBAL_TRANS_CHECK_BYPASS).at(*ScriptLocal(GtaThread::fromHash(ATSTRINGHASH("maintransition")), LOCAL_MT_LFF + LOCAL_MT_LFF_CHECKIDX).as<int*>(), 1).set(TRUE);
				}
			}
		}
	}

	void CommandSeamless::onPostTick()
	{
		if (!canRun())
		{
			return;
		}
		AbstractEntity::updatePlayerPedVehAndEnt();
		const auto ts = ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>();
		if (is_session_transition_active(ts))
		{
			if (!prev_transition_active) // Transition just started?
			{
				prev_transition_active = true;

				online_world_state_desired = !(ScriptGlobal(GLOBAL_NEW_SESSION_QUIT_TO_SP).get<int>() & 1);
				online_world_state_reality = WorldState::getOnline();
				ScriptGlobal(GLOBAL_MP_WORLD_STATE).set<BOOL>(online_world_state_desired);

				// Save player's vehicle
				TranscendentVehicle::saveFromPlayerState();
			}
			else // Maintaining transition?
			{
				// Maintain player's vehicle
				if (!g_player_veh.isValid()
					&& TranscendentVehicle::active
					)
				{
					if (auto veh = g_player_ped.getVehicle(true); veh.isValid())
					{
						//Util::toast("Lost vehicle; but it still exists; remounting.");
						PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, -1);
					}
					else
					{
						//Util::toast(fmt::format("Lost vehicle during {}", (int)ScriptGlobal(GLOBAL_TRANSITION_STATE).get<TransitionState>()));
						AbstractEntity::getAllVehicles([](AbstractEntity&& veh)
						{
							if (veh.getPos().distance(g_player_ped.getPos()) < 20.0f
								&& TranscendentVehicle::cust_data.read(VehicleCustomisationIo::MODEL).get<Hash>() == veh.getModelHash()
								)
							{
								//Util::toast("Refound vehicle");
								PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, -1);
								g_player_veh = veh;
								AbstractEntity::updatePlayerEnt();
								CONSUMER_BREAK;
							}
							CONSUMER_CONTINUE;
						});
						if (!g_player_veh.isValid())
						{
							//Util::toast("Could not find it; recovering.");
							TranscendentVehicle::recover();
						}
					}
				}
			}

			if (!g_tb_screenshot_mode.isEnabled())
			{
				*pointers::hide_hud_and_radar_this_frame = false;
			}

			PLAYER::STOP_PLAYER_TELEPORT();
			PLAYER::SET_PLAYER_CONTROL(g_player, TRUE, 0);
			ENTITY::SET_ENTITY_VISIBLE(g_player_ped, TRUE, FALSE);
			ENTITY::SET_ENTITY_COLLISION(g_player_ped, TRUE, FALSE);
			ENTITY::FREEZE_ENTITY_POSITION(g_player_ped, FALSE);
			STREAMING::SET_FOCUS_ENTITY(g_player_ped);

			*ScriptGlobal(GLOBAL_TRANS_FLAGS).as<int*>() |= (1 << 16); // prevents scripts freezing us / resetting momentum during create player

			if (HUD::IS_WARNING_MESSAGE_ACTIVE())
			{
				if (warning_message_since == 0)
				{
					warning_message_since = get_current_time_millis();
				}
				else if (GET_MILLIS_SINCE(warning_message_since) > 3000)
				{
					*ScriptGlobal(GLOBAL_NEW_SESSION).as<int*>() = 0;
					ScriptGlobal(GLOBAL_TRANSITION_STATE).set(TRANSITION_STATE_TERMINATE_MAINTRANSITION);
				}
			}
			else
			{
				warning_message_since = 0;
			}

			ScriptGlobal(GLOBAL_LOADING_HINT).set<int>(2); // 0 = tip, 1 = news, 2+ = nothing
			if (HUD::IS_PAUSE_MENU_ACTIVE())
			{
				HUD::SET_FRONTEND_ACTIVE(FALSE);
			}

			GRAPHICS::TOGGLE_PAUSED_RENDERPHASES(TRUE);
			GRAPHICS::RESET_PAUSED_RENDERPHASES();

			GRAPHICS::SET_HIDOF_OVERRIDE(0, 0, 0.0f, 0.0f, FLT_MAX, FLT_MAX);
		}
		else
		{
			if (prev_transition_active)
			{
				prev_transition_active = false;

				ScriptGlobal(GLOBAL_MP_WORLD_STATE).set<BOOL>(online_world_state_reality);
				if (WorldState::getOnline() != online_world_state_desired)
				{
					bool sync = false;
					switch (world_state_config->value)
					{
					case 1:
						sync = true;
						break;

					case 2:
						sync = online_world_state_desired;
						break;

					case 3:
						sync = !online_world_state_desired;
						break;
					}
					if (sync)
					{
						FiberPool::queueJob([]
						{
							Script::current()->yield(5000);
							if (is_session_transition_active(true))
							{
								return;
							}
							centered_text = fmt::format(fmt::runtime(LANG_GET_W("WRLDSTCNG_T")), 3);
							Script::current()->yield(1000);
							centered_text = fmt::format(fmt::runtime(LANG_GET_W("WRLDSTCNG_T")), 2);
							Script::current()->yield(1000);
							centered_text = fmt::format(fmt::runtime(LANG_GET_W("WRLDSTCNG_T")), 1);
							Script::current()->yield(1000);
							centered_text.clear();
							g_tb_stare_at_sky.enable();
							Script::current()->yield();
							WorldState::setOnline(online_world_state_desired);
							g_tb_stare_at_sky.disable();
						});
					}
				}

				// If we came into the session with a vehicle, we'll have to keep a watch for possible disruptions.
				if (TranscendentVehicle::active)
				{
					TranscendentVehicle::active = false;
					FiberPool::queueJob([]
					{
						auto deadline = get_current_time_millis() + 10000;
						do
						{
							// Ensure user is not kicked from their vehicle
							DECORATOR::DECOR_REMOVE(g_player_veh, "Player_Vehicle");

							// Ensure user stays in control of their ped
							if (TASK::GET_IS_TASK_ACTIVE(g_player_ped, CTaskTypes::TASK_CAR_DRIVE_WANDER))
							{
								TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
								PED::SET_PED_INTO_VEHICLE(g_player_ped, g_player_veh, -1);
							}

							Script::current()->yield();
						} while (!IS_DEADLINE_REACHED(deadline));
					});
				}
			}
		}
		prev_ts = ts;
	}

	void CommandSeamless::onPresent()
	{
		if (!centered_text.empty())
		{
			g_renderer.drawCentredTextThisFrame(centered_text);
		}
	}

	void CommandSeamless::onEnable(Click& click)
	{
		NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("freemode"), 0x1AC8F4AD40E22127, &nullsub<>);
		NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("freemode"), 0x1DB21A44B09E8BA3, &nullsub<>);
		reScriptTickEvent::registerHandler(&onPreTick);
		rePostTickEvent::registerHandler(&onPostTick);
		rePresentEvent::registerHandler(&onPresent);
	}

	void CommandSeamless::onDisable(Click& click)
	{
		NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("freemode"), 0x1AC8F4AD40E22127);
		NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("freemode"), 0x1DB21A44B09E8BA3);
		reScriptTickEvent::unregisterHandler(&onPreTick);
		rePostTickEvent::unregisterHandler(&onPostTick);
		rePresentEvent::unregisterHandler(&onPresent);
	}
}
