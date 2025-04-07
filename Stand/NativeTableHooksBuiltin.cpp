#include "NativeTableHooksBuiltin.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "CNetGamePlayer.hpp"
#include "Commandbox.hpp"
#include "CommandPlayer.hpp"
#include "eEventType.hpp"
#include "EventAggregation.hpp"
#include "Exceptional.hpp"
#include "FlowEvent.hpp"
#include "FlowEventReaction.hpp"
#include "fmt_arg.hpp"
#include "Hooking.hpp"
#include "LeaveReasons.hpp"
#include "natives.hpp"
#include "NativeTableHooks.hpp"
#include "ScriptLocal.hpp"
#include "script_thread.hpp"
#include "SharedNativeHooks.hpp"
#include "Util.hpp"

#ifdef STAND_DEBUG
#include "script_program.hpp"
#endif

namespace Stand
{
	static int last_event_group = -1;
	static int last_event_index = -1;
	static int last_event_type = -1;

	[[nodiscard]] static AbstractPlayer bootstrap_join_leave_event(int* args, int args_size)
	{
		if (args_size == 56 && args[18 * 2] == 0)
		{
			for (int i = 0; i < args[19 * 2]; i++)
			{
				if (args[(20 + 1 + i) * 2] == SCRIPT::GET_ID_OF_THIS_THREAD())
				{
					return args[16 * 2]; // player name = (const char*)&args[0]
				}
			}
		}
		return AbstractPlayer::invalid();
	}

	void NativeTableHooksBuiltin::init()
	{
		// START_NEW_SCRIPT_WITH_ARGS
		NativeTableHooks::createNativeEntrypointOverride(ATSTRINGHASH("am_launcher"), 0xB8BA7F44DF1575E1, [](rage::scrNativeCallContext& ctx)
		{
			auto thread = rage::scrThread::get();
			if (*ScriptLocal(thread, LOCAL_LNCH_PLAYERSTATE).at(g_player, 3).at(2).as<int*>() == 6)
			{
				const auto script_hash = rage::atStringHash(ctx.getArg<const char*>(0));
				AbstractPlayer host_ap = thread->getHost();
				EventAggregation t(FlowEvent::SCRLAUNCH_ANY, LOC("SESSSCRSTRT"));
				const auto script_name = rage::scrThread::getLabel(script_hash).getLocalisedUtf8();
				switch (script_hash)
				{
					// Freemode Activities
				case ATSTRINGHASH("AM_KILL_LIST"):
				case ATSTRINGHASH("AM_CP_COLLECTION"):
				case ATSTRINGHASH("AM_CHALLENGES"):
				case ATSTRINGHASH("AM_PENNED_IN"):
				case ATSTRINGHASH("AM_PASS_THE_PARCEL"):
				case ATSTRINGHASH("AM_HOT_PROPERTY"):
				case ATSTRINGHASH("AM_KING_OF_THE_CASTLE"):
				case ATSTRINGHASH("AM_CRIMINAL_DAMAGE"):
				case ATSTRINGHASH("AM_HUNT_THE_BEAST"):
				case ATSTRINGHASH("BUSINESS_BATTLES"):
				case ATSTRINGHASH("fm_content_business_battles"):
					t.addRaw(FlowEvent::SCRLAUNCH_FE, script_name);
					break;

					// Arcade Games
				case ATSTRINGHASH("gunslinger_arcade"):
				case ATSTRINGHASH("camhedz_arcade"):
				case ATSTRINGHASH("road_arcade"):
				case ATSTRINGHASH("ggsm_arcade"):
				case ATSTRINGHASH("wizard_arcade"):
				case ATSTRINGHASH("puzzle"):
					t.addRaw(FlowEvent::SCRLAUNCH_AG, script_name);
					break;

					// Removed Freemode Activities
				case ATSTRINGHASH("AM_DEAD_DROP"):
				case ATSTRINGHASH("AM_HOT_TARGET"):
				case ATSTRINGHASH("AM_PLANE_TAKEDOWN"):
				case ATSTRINGHASH("AM_DISTRACT_COPS"):
				case ATSTRINGHASH("AM_DESTROY_VEH"):
					t.addRaw(FlowEvent::SCRLAUNCH_OFM, script_name);
					break;

					// Session Breaking
				case ATSTRINGHASH("FM_INTRO"):
				case ATSTRINGHASH("fm_mission_controller"):
				case ATSTRINGHASH("golf_mp"):
				case ATSTRINGHASH("FM_Survival_Controller"):
				case ATSTRINGHASH("FM_Horde_Controler"):
				case ATSTRINGHASH("fm_deathmatch_controler"):
				case ATSTRINGHASH("tennis_network_mp"):
				case ATSTRINGHASH("Range_Modern_MP"):
				case ATSTRINGHASH("FM_Race_Controler"):
				case ATSTRINGHASH("fm_Bj_race_controler"):
				case ATSTRINGHASH("fm_content_ufo_abduction"):
					t.addRaw(FlowEvent::SCRLAUNCH_GRIEF, script_name);
					break;

					// Services
				case ATSTRINGHASH("AM_airstrike"):
				case ATSTRINGHASH("AM_AMMO_DROP"):
				case ATSTRINGHASH("AM_backup_heli"):
				case ATSTRINGHASH("AM_ISLAND_BACKUP_HELI"):
				case ATSTRINGHASH("AM_BOAT_TAXI"):
				case ATSTRINGHASH("AM_HELI_TAXI"):
				case ATSTRINGHASH("AM_BRU_BOX"):
					t.addRaw(FlowEvent::SCRLAUNCH_SERV, script_name);
					break;

					// Cutscenes
				case ATSTRINGHASH("am_heist_int"):
				case ATSTRINGHASH("am_lester_cut"):
				case ATSTRINGHASH("am_ronTrevor_Cut"):
					t.addRaw(FlowEvent::SCRLAUNCH_CUT, script_name);
					break;

				case ATSTRINGHASH("MG_RACE_TO_POINT"):
					t.add(FlowEvent::SCRLAUNCH_FOIM);
					break;

				case ATSTRINGHASH("Pilot_School_MP"):
					t.add(FlowEvent::SCRLAUNCH_PS);
					break;

				case ATSTRINGHASH("fm_content_sightseeing"):
					t.add(FlowEvent::SCRLAUNCH_STRIKE);
					break;

				case ATSTRINGHASH("fm_hideout_controler"):
					t.add(FlowEvent::SCRLAUNCH_DPSSV);
					break;

				case ATSTRINGHASH("AM_Darts"):
					t.add(FlowEvent::SCRLAUNCH_DARTS);
					break;

				case ATSTRINGHASH("FM_Impromptu_DM_Controler"):
					t.add(FlowEvent::SCRLAUNCH_IMPDM);
					break;

				case ATSTRINGHASH("fm_content_slasher"):
					t.add(FlowEvent::SCRLAUNCH_SLASHER);
					break;

				default:
					t.addRaw(FlowEvent::SCRLAUNCH_NOCAT, script_name);
					break;
				}
				auto reactions = t.getReactions(host_ap);
				if (auto toast_flags = flow_event_reactions_to_toast_flags(reactions))
				{
					Util::toast(LANG_FMT(
						"SESSSCRSTRT_T",
						FMT_ARG("player", thread->getHost().getName()),
						FMT_ARG("script", script_name)
					), toast_flags);
				}
				host_ap.applyReactions(reactions, std::move(t));
				if (reactions & REACTION_BLOCK)
				{
					ctx.setReturnValue<int>(1);
					return;
				}
			}
			return NativeTableHooks::og(0xB8BA7F44DF1575E1)(ctx);
		});

		// GET_EVENT_AT_INDEX(int eventGroup, int eventIndex)
		NativeTableHooks::createNativeEntrypointOverride(0xD8F66A3A60C62153, [](rage::scrNativeCallContext& ctx)
		{
			last_event_group = ctx.getArg<int>(0);
			last_event_index = ctx.getArg<int>(1);
			NativeTableHooks::og(0xD8F66A3A60C62153)(ctx);
			last_event_type = ctx.getReturnValue<int>();
		});

		// GET_EVENT_DATA(int eventGroup, int eventIndex, Any* eventData, int eventDataSize)
		NativeTableHooks::createNativeEntrypointOverride(0x2902843FCD2B2D79, [](rage::scrNativeCallContext& ctx)
		{
			__try
			{
				NativeTableHooks::og(0x2902843FCD2B2D79)(ctx);
				if (!ctx.getReturnValue<BOOL>())
				{
					return;
				}
				const auto event_group = ctx.getArg<int>(0);
				const auto event_index = ctx.getArg<int>(1);
				const auto args = ctx.getArg<int*>(2);
				const auto args_size = ctx.getArg<int>(3);
				if (event_group == 1 && event_group == last_event_group && event_index == last_event_index)
				{
					switch (last_event_type)
					{
					case EVENT_NETWORK_PLAYER_JOIN_SCRIPT:
						if (rage::tlsContext::get()->m_script_thread->m_context.m_script_hash == ATSTRINGHASH("freemode"))
						{
							AbstractPlayer p = bootstrap_join_leave_event(args, args_size);
							if (p.isValid())
							{
								auto* const command = p.getCommand();
								if (command != nullptr)
								{
									command->joined = true;
									//Util::toast(fmt::format("{} marked as joined {} ms after discovery", p.getName(), GET_MILLIS_SINCE(command->discovery)));
								}
							}
						}
						break;

					case EVENT_NETWORK_PLAYER_LEFT_SCRIPT:
					{
						AbstractPlayer p = bootstrap_join_leave_event(args, args_size);
						if (p.isValid())
						{
							if (rage::tlsContext::get()->m_script_thread->m_context.m_script_hash == ATSTRINGHASH("freemode"))
							{
								if (auto gamer_info = p.getGamerInfoNoFallback())
								{
									LeaveReasons::getEntry(*gamer_info).left_freemode = true;
								}
							}
						}
					}
					break;

					case EVENT_NETWORK_SCRIPT_EVENT:
						if (args_size > 1 * 2)
						{
							int32_t script_hash = rage::scrThread::get()->m_context.m_script_hash;
							auto script_i = g_hooking.last_script_event_map.find(script_hash);
							auto args_copy = new int[args_size * 2];
							memcpy(args_copy, args, args_size * 8);
							auto pair = std::pair<int, std::unique_ptr<int[]>>(args_size, args_copy);
							if (script_i == g_hooking.last_script_event_map.end())
							{
								g_hooking.last_script_event_map.emplace(std::move(script_hash), std::move(pair));
							}
							else
							{
								script_i->second = std::move(pair);
							}

							/*if (args[0 * 2] == SE_PV_CLEANUP)
							{
								args[1 * 2] = 1;
							}*/
						}
						break;
					}
				}
			}
			__EXCEPTIONAL()
			{
			}
		});

		// NETWORK_BAIL
		NativeTableHooks::createNativeEntrypointOverride(0x95914459A87EBA28, [](rage::scrNativeCallContext& ctx)
		{
			__try
			{
				const auto a1 = ctx.getArg<int>(0);
				const auto a2 = ctx.getArg<int>(1);
				const auto a3 = ctx.getArg<int>(2);

				bool block = g_hooking.block_bail_other;
				if (a2 == 0 && a3 == 0)
				{
					if (a1 == 15 || a1 == 17)
					{
						block = g_hooking.block_bail_spectating;
					}
					else if (a1 == 26)
					{
						block = g_hooking.block_bail_rid;
					}
				}
				Util::toast(LANG_FMT("PTX_NB_T", FMT_ARG("a1", a1), FMT_ARG("a2", a2), FMT_ARG("a3", a3), FMT_ARG("action", block ? LANG_GET("PTX_NB_T_B") : LANG_GET("PTX_NB_T_A"))), TOAST_ALL);
				if (!block)
				{
					NativeTableHooks::og(0x95914459A87EBA28)(ctx);
				}
			}
			__EXCEPTIONAL()
			{
			}
		});

		// IS_DISABLED_CONTROL_PRESSED
		NativeTableHooks::createNativeEntrypointOverride(0xE2587F8CBBD87B1D, [](rage::scrNativeCallContext& ctx)
		{
			SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
			{
				return ctx.setReturnValue<>(FALSE);
			}
			return NativeTableHooks::og(0xE2587F8CBBD87B1D)(ctx);
		});

		// IS_DISABLED_CONTROL_RELEASED
		NativeTableHooks::createNativeEntrypointOverride(0xFB6C4072E9A32E92, [](rage::scrNativeCallContext& ctx)
		{
			SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
			{
				return ctx.setReturnValue<>(FALSE);
			}
			return NativeTableHooks::og(0xFB6C4072E9A32E92)(ctx);
		});

		// IS_DISABLED_CONTROL_JUST_PRESSED
		NativeTableHooks::createNativeEntrypointOverride(0x91AEF906BCA88877, [](rage::scrNativeCallContext& ctx)
		{
			SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
			{
				return ctx.setReturnValue<>(FALSE);
			}
			return NativeTableHooks::og(0x91AEF906BCA88877)(ctx);
		});

		// IS_DISABLED_CONTROL_JUST_RELEASED
		NativeTableHooks::createNativeEntrypointOverride(0x305C8DCD79DA8B0F, [](rage::scrNativeCallContext& ctx)
		{
			SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
			{
				return ctx.setReturnValue<>(FALSE);
			}
			return NativeTableHooks::og(0x305C8DCD79DA8B0F)(ctx);
		});

		// SET_INPUT_EXCLUSIVE
		NativeTableHooks::createNativeEntrypointOverride(0xEDE476E5EE29EDB1, [](rage::scrNativeCallContext& ctx)
		{
			SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
			{
				return;
			}
			return NativeTableHooks::og(0xEDE476E5EE29EDB1)(ctx);
		});

		// GET_DISABLED_CONTROL_NORMAL
		NativeTableHooks::createNativeEntrypointOverride(0x11E65974A982637C, [](rage::scrNativeCallContext& ctx)
		{
			SOUP_IF_UNLIKELY (g_commandbox.shouldBlockGameInputs())
			{
				switch (ctx.getArg<int>(1))
				{
				case 239:
				case 240:
					return ctx.setReturnValue<>(0.5f);
				}
				return ctx.setReturnValue<>(0.0f);
			}
			return NativeTableHooks::og(0x11E65974A982637C)(ctx);
		});

		// SC_INBOX_GET_BOUNTY_DATA_AT_INDEX
		NativeTableHooks::createNativeEntrypointOverride(0x87E0052F08BD64E6, [](rage::scrNativeCallContext& ctx)
		{
			// Purpose: Silently block invalid "~a~ survived the $~1~ Bounty you placed on them." notifications sent via SocialClub inbox message.
			NativeTableHooks::og(0x87E0052F08BD64E6)(ctx);
			auto bounty_value = ctx.getArg<int*>(1)[17 * 2];
			if (bounty_value != 1000 && bounty_value != 3000 && bounty_value != 5000 && bounty_value != 7000 && bounty_value != 9000)
			{
				//Util::toast(fmt::format("Bad bounty value: {}", bounty_value), TOAST_ALL);
				ctx.getArg<int*>(1)[16 * 2] = 0; // clear bitflag so it does nothing with this
			}
		});

		// UGC_GET_CONTENT_NAME
		NativeTableHooks::createNativeEntrypointOverride(0xBF09786A7FCAB582, [](rage::scrNativeCallContext& ctx)
		{
			NativeTableHooks::og(0xBF09786A7FCAB582)(ctx);
			auto desc = StringUtils::utf8_to_utf16(ctx.getReturnValue<const char*>());
			AbstractPlayer::normaliseForAdFilters(desc);
			if (AbstractPlayer::messageHasCallToAction(desc) || AbstractPlayer::messageHasPromiseOfGoodThings(desc))
			{
				ctx.setReturnValue<const char*>(HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION("CREATOR_NO_T"));
			}
		});

		// UGC_GET_CACHED_DESCRIPTION
		NativeTableHooks::createNativeEntrypointOverride(0x40F7E66472DF3E5C, [](rage::scrNativeCallContext& ctx)
		{
			NativeTableHooks::og(0x40F7E66472DF3E5C)(ctx);
			auto desc = StringUtils::utf8_to_utf16(ctx.getReturnValue<const char*>());
			AbstractPlayer::normaliseForAdFilters(desc);
			if (AbstractPlayer::messageHasCallToAction(desc) || AbstractPlayer::messageHasPromiseOfGoodThings(desc))
			{
				ctx.setReturnValue<const char*>(HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION("CREATOR_NO_T"));
			}
		});

		// CREATE_NON_NETWORKED_AMBIENT_PICKUP
		NativeTableHooks::createNativeEntrypointOverride(0x9C93764223E29C50, [](rage::scrNativeCallContext& ctx)
		{
			auto thread_ctx = &GtaThread::get()->m_context;
			SOUP_IF_LIKELY (thread_ctx->m_script_hash != ATSTRINGHASH("freemode")
				|| Util::is_legit_ambient_pickup(ctx.getArg<Hash>(0), ctx.getArg<int>(5), ctx.getArg<Hash>(6))
				)
			{
				return NativeTableHooks::og(0x9C93764223E29C50)(ctx);
			}
#ifdef STAND_DEBUG
			auto program = rage::scrProgram::fromHash(thread_ctx->m_script_hash);
			Util::toast(fmt::format("[Debug Build] Blocked {} creating a pickup in func_{}", program->m_name, program->getFuncIndexByCodeIndex(thread_ctx->m_instruction_pointer) - 1), TOAST_ALL);
#elif defined(STAND_DEV)
			Util::toast(fmt::format(fmt::runtime(soup::ObfusString("[Dev Build] Blocked {} creating a pickup").str()), (const char*)GtaThread::get()->m_name));
#endif
		});

		// NETWORK_SKIP_RADIO_WARNING
		NativeTableHooks::createNativeEntrypointOverride(0x659CF2EF7F550C4F, [](rage::scrNativeCallContext& ctx)
		{
			ctx.setReturnValue<BOOL>(FALSE);
		});

		// SET_ENTITY_INVINCIBLE
		NativeTableHooks::createNativeEntrypointOverride(0x3882114BDE571AD4, [](rage::scrNativeCallContext& ctx)
		{
			//Util::toast(fmt::format("SET_ENTITY_INVINCIBLE: {} -> {}", ctx.getArg<Entity>(0), ctx.getArg<BOOL>(1)));
			if (g_player_ped == ctx.getArg<Entity>(0))
			{
				g_hooking.have_legit_invincible = ctx.getArg<BOOL>(1);
			}
			return NativeTableHooks::og(0x3882114BDE571AD4)(ctx);
		});

		// SET_ENTITY_CAN_BE_DAMAGED
		NativeTableHooks::createNativeEntrypointOverride(0x1760FFA8AB074D66, [](rage::scrNativeCallContext& ctx)
		{
			//Util::toast(fmt::format("SET_ENTITY_CAN_BE_DAMAGED: {} -> {}", ctx.getArg<Entity>(0), ctx.getArg<BOOL>(1)));
			if (g_player_ped == ctx.getArg<Entity>(0))
			{
				g_hooking.have_legit_invincible = !ctx.getArg<BOOL>(1);
			}
			return NativeTableHooks::og(0x1760FFA8AB074D66)(ctx);
		});

		// SET_ENTITY_PROOFS
		NativeTableHooks::createNativeEntrypointOverride(0xFAEE099C6F890BB8, [](rage::scrNativeCallContext& ctx)
		{
			if (g_player_ped == ctx.getArg<Entity>(0))
			{
				g_hooking.have_legit_bullet_proof = ctx.getArg<BOOL>(1);
				g_hooking.have_legit_flame_proof = ctx.getArg<BOOL>(2);
				g_hooking.have_legit_explosion_proof = ctx.getArg<BOOL>(3);
				g_hooking.have_legit_collision_proof = ctx.getArg<BOOL>(4);
				g_hooking.have_legit_melee_proof = ctx.getArg<BOOL>(5);
				g_hooking.have_legit_steam_proof = ctx.getArg<BOOL>(6);
				// arg 7 is bDontResetDamageFlagsOnCleanupMissionState
				g_hooking.have_legit_smoke_proof = ctx.getArg<BOOL>(8);
			}
			return NativeTableHooks::og(0xFAEE099C6F890BB8)(ctx);
		});

		NativeTableHooks::createNativeEntrypointOverride(0x7F7E8401F81CB65B, [](rage::scrNativeCallContext& ctx)
		{
			ctx.setReturnValue<int>(0);
		});

		SharedNativeHooks::enable([](XoredNativeHash target, rage::scrNativeHandler detour, rage::scrNativeHandler* pOriginal)
		{
			NativeTableHooks::createNativeEntrypointOverride(target, detour);
		});
	}

	void NativeTableHooksBuiltin::deinit()
	{
		NativeTableHooks::removeNativeEntrypointOverride(ATSTRINGHASH("am_launcher"), 0xB8BA7F44DF1575E1);
		NativeTableHooks::removeNativeEntrypointOverride(0xD8F66A3A60C62153);
		NativeTableHooks::removeNativeEntrypointOverride(0x2902843FCD2B2D79);
		NativeTableHooks::removeNativeEntrypointOverride(0x95914459A87EBA28);
		NativeTableHooks::removeNativeEntrypointOverride(0xE2587F8CBBD87B1D);
		NativeTableHooks::removeNativeEntrypointOverride(0xFB6C4072E9A32E92);
		NativeTableHooks::removeNativeEntrypointOverride(0x91AEF906BCA88877);
		NativeTableHooks::removeNativeEntrypointOverride(0x305C8DCD79DA8B0F);
		NativeTableHooks::removeNativeEntrypointOverride(0xEDE476E5EE29EDB1);
		NativeTableHooks::removeNativeEntrypointOverride(0x11E65974A982637C);
		NativeTableHooks::removeNativeEntrypointOverride(0x87E0052F08BD64E6);
		NativeTableHooks::removeNativeEntrypointOverride(0xBF09786A7FCAB582);
		NativeTableHooks::removeNativeEntrypointOverride(0x40F7E66472DF3E5C);
		NativeTableHooks::removeNativeEntrypointOverride(0x9C93764223E29C50);
		NativeTableHooks::removeNativeEntrypointOverride(0x659CF2EF7F550C4F);
		NativeTableHooks::removeNativeEntrypointOverride(0x3882114BDE571AD4);
		NativeTableHooks::removeNativeEntrypointOverride(0x1760FFA8AB074D66);
		NativeTableHooks::removeNativeEntrypointOverride(0xFAEE099C6F890BB8);
		NativeTableHooks::removeNativeEntrypointOverride(0x7F7E8401F81CB65B);

		SharedNativeHooks::disable([](XoredNativeHash target, rage::scrNativeHandler original)
		{
			NativeTableHooks::removeNativeEntrypointOverride(target, original);
		});
	}
}
