#include "CommandPlayerMuggerLoop.hpp"

#include "AbstractEntity.hpp"
#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "natives.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "script_handler.hpp"
#include "ScriptHostUtil.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"

#define MUGDEBUG false

#if MUGDEBUG
#include "drawDebugText.hpp"
#include "DrawUtil3d.hpp"
#include "ThreadContext.hpp"
#include "Util.hpp"
#endif

namespace Stand
{
	CommandPlayerMuggerLoop::CommandPlayerMuggerLoop(CommandList* const parent)
		: CommandPlayerToggle(parent, LOC("PLYMUGLOP"), { CMDNAME("mugloop") }, NOLABEL, COMMANDPERM_AGGRESSIVE)
	{
		this->flags |= CMDFLAG_FEATURELIST_ULTIMATE;
	}

	static int current_mugloop_target = MAX_PLAYERS;
	static constexpr auto gang_type = 0; // mugger

	static void requestMugger(const int target)
	{
		*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_GANGCALLOWNER).as<int*>() |= (1 << gang_type);
		*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_GANGCALLTARGET).as<int*>() = target;
		GtaThread::fromHash(ATSTRINGHASH("freemode"))->fiberSnatchScriptHost();
		*ScriptGlobal(GlobalServerBD_FM + iGangCallCurrentOwnerPlayer).at(gang_type, 1).as<int*>() = target;
		*ScriptGlobal(GlobalServerBD_FM + iGangCallCurrentTargetPlayer).at(gang_type, 1).as<int*>() = target;
	}

	static void setUnrequestGlobals()
	{
		*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_GANGCALLOWNER).as<int*>() &= ~(1 << gang_type);
		*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_GANGCALLTARGET).as<int*>() = -1;
		GtaThread::fromHash(ATSTRINGHASH("freemode"))->fiberSnatchScriptHost();
		*ScriptGlobal(GlobalServerBD_FM + iGangCallCurrentOwnerPlayer).at(gang_type, 1).as<int*>() = -1;
		*ScriptGlobal(GlobalServerBD_FM + iGangCallCurrentTargetPlayer).at(gang_type, 1).as<int*>() = -1;

		*ScriptGlobal(GLOBAL_PLAYER_STAT_BASE).at(g_player, GLOBAL_PLAYER_STAT_SIZE).at(GLOBAL_PLAYER_STAT_GANGCALLSEENRUNNING).as<int*>() = 0;
	}

	static void unrequestMuggerImpl()
	{
		setUnrequestGlobals();
		if (auto t = GtaThread::fromHash(ATSTRINGHASH("AM_GANG_CALL")))
		{
			t->fiberSnatchScriptHost();
			*ScriptLocal(t, LOCAL_GANGCALL_SHDATA + LOCAL_GANGCALL_STATE).as<int*>() = 4;
			setUnrequestGlobals();
		}
	}

	static void unrequestMugger()
	{
		unrequestMuggerImpl();
		Script::current()->yield(1000);
	}

	static void mugger_loop(const int target)
	{
		unrequestMugger();

		time_t requested_at = 0;
		time_t started_at = 0;
		while (current_mugloop_target == target)
		{
			if (auto t = GtaThread::fromHash(ATSTRINGHASH("AM_GANG_CALL")))
			{
				requested_at = 0;
				if (started_at == 0)
				{
					started_at = get_current_time_millis();
				}

				/*const auto participants = (t->m_net_component ? t->m_net_component->GetNumParticipants() : 0);
				if (GET_MILLIS_SINCE(started_at) > 3000)
				{
					if (participants <= 1)
					{
#if MUGDEBUG
						Util::toast("We're the only participant, terminating.", TC_OTHER);
#endif
						MISC::TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME("AM_GANG_CALL");
						unrequestMugger();
						continue;
					}
				}*/

				if (GET_MILLIS_SINCE(started_at) > 10000)
				{
#if MUGDEBUG
					Util::toast("Mugger took more than 10s, cancelling.", TC_OTHER);
#endif
					unrequestMugger();
					continue;
				}

#if MUGDEBUG
				drawDebugText(fmt::format("AM_GANG_CALL running with {} participants", participants));
#endif

				if (t->hostedByUser())
				{
#if MUGDEBUG
					drawDebugText("Hosting it");
#endif

					// IS_BIT_SET(serverBd.iBitset, SERVER_BS_OK_TO_SPAWN_GANG_NOW)
					*ScriptLocal(t, LOCAL_GANGCALL_SHDATA + LOCAL_GANGCALL_BITSET).as<int*>() |= (1 << 7);
				}
				else
				{
					t->snatchScriptHost();
				}

				auto mugger_netId = *ScriptLocal(t, LOCAL_GANGCALL_SHDATA + LOCAL_GANGCALL_GANGMEMBER).at(0, LOCAL_GANGMEMBER_SIZE).at(0).as<int*>();
				auto mugger_state = *ScriptLocal(t, LOCAL_GANGCALL_SHDATA + LOCAL_GANGCALL_GANGMEMBER).at(0, LOCAL_GANGMEMBER_SIZE).at(5).as<int*>();
				Ped mugger_ped = 0;
				t->spoofAs([&]
				{
					mugger_ped = NETWORK::NET_TO_PED(mugger_netId);
				});

#if MUGDEBUG
				drawDebugText(mugger_state);
				drawDebugText(mugger_netId);
				drawDebugText(mugger_ped);
#endif

				if (mugger_state == 3)
				{
#if MUGDEBUG
					Util::toast("Mugger is fleeing, cancelling.", TC_OTHER);
#endif
					unrequestMugger();
					continue;
				}

				if (mugger_ped)
				{
#if MUGDEBUG
					DrawUtil3d::draw_ar_marker(AbstractEntity::get(mugger_ped).getPos());
#endif

					t->spoofAs([&]
					{
						if (NETWORK::NETWORK_HAS_CONTROL_OF_NETWORK_ID(mugger_netId))
						{
							AbstractEntity::get(mugger_ped).godmodeEnable();

							auto target_pos = AbstractPlayer(target).getPos();
							if (target_pos.distance(AbstractEntity::get(mugger_ped).getPos()) > 5.0f)
							{
								target_pos.x += 1.0f;
								AbstractEntity::get(mugger_ped).setPos(target_pos);
							}
						}
						else
						{
							NETWORK::NETWORK_REQUEST_CONTROL_OF_NETWORK_ID(mugger_netId);
						}
					});
				}
			}
			else
			{
				//drawDebugText(NETWORK::NETWORK_IS_SCRIPT_ACTIVE("AM_GANG_CALL", 0, TRUE, 0));
				if (GET_MILLIS_SINCE(requested_at) > 5000)
				{
					if (requested_at != 0)
					{
#if MUGDEBUG
						Util::toast("Mugger script not active after 5 seconds, restarting.", TC_OTHER);
#endif
						unrequestMugger();
					}
					else if (started_at == 0)
					{
#if MUGDEBUG
						Util::toast("Mugger script not active, starting it.", TC_OTHER);
#endif
					}
					else
					{
#if MUGDEBUG
						Util::toast("Mugger script just ended, waiting a bit then restarting.", TC_OTHER);
#endif
						Script::current()->yield(3000);
					}
					requestMugger(target);
					requested_at = get_current_time_millis();
				}
				started_at = 0;
			}
			Script::current()->yield();
		}
	}

	void CommandPlayerMuggerLoop::onEnable(Click& click)
	{
		const int target = PP_PTR->getPlayer();
		if (target == g_player)
		{
			click.notAvailableOnUser();
			onChangeImplUnavailable(click);
			return;
		}
		if (!click.isUltimateEdition()
			|| !ScriptHostUtil::canSafelyTakeIt()
			)
		{
			onChangeImplUnavailable(click);
			return;
		}
		FiberPool::queueJob([target]
		{
			current_mugloop_target = target;
			mugger_loop(target);
		});
	}

	void CommandPlayerMuggerLoop::onDisable(Click& click)
	{
		if (current_mugloop_target != MAX_PLAYERS)
		{
			current_mugloop_target = MAX_PLAYERS;
			FiberPool::queueJob([]
			{
				unrequestMuggerImpl();
			});
		}
	}
}
