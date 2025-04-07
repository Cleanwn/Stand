#include "SharedNativeHooks.hpp"

#include "AbstractEntity.hpp"
#include "Exceptional.hpp"
#include "gta_entity.hpp"
#include "Hooking.hpp"
#include "is_session.hpp"
#include "NativeTableHooks.hpp"
#include "script_events.hpp"
#include "ScriptEventTaxonomy.hpp"
#include "script_thread.hpp"

#ifdef STAND_DEBUG
#include "script_program.hpp"
#include "Util.hpp"
#endif

namespace Stand
{
	static rage::scrNativeHandler SEND_TU_SCRIPT_EVENT_ogfp;
	static void SEND_TU_SCRIPT_EVENT(rage::scrNativeCallContext& ctx)
	{
		__try
		{
			auto& event_group = ctx.getArg<int>(0);
			auto& args = ctx.getArg<int64_t*>(1);
			auto& args_count = ctx.getArg<int>(2);
			auto& player_bitflags = ctx.getArg<unsigned int>(3);

			if (event_group == 1)
			{
				const Player my_player_id = (is_session_transition_active(true) ? MAX_PLAYERS : g_player.operator Player());
				const bool send_to_self = (my_player_id < MAX_PLAYERS && player_bitflags == (1 << my_player_id));
				SOUP_ASSERT(args_count > 0);
				switch ((int32_t)args[0])
				{
				case SE_START_TYPING:
				case SE_STOP_TYPING:
					if (SharedNativeHooks::block_outgoing_typing_indicator_events)
					{
						return;
					}
					break;

#ifdef STAND_DEBUG
				case SE_PV_KICK:
					if (send_to_self)
					{
						auto thread_ctx = &GtaThread::get()->m_context;
						auto program = rage::scrProgram::fromHash(thread_ctx->m_script_hash);
						Util::toast(fmt::format("Kick from vehicle ({}, {}) sent by {} in func_{}", (int32_t)args[4], (float)(int32_t)args[2], program->m_name, program->getFuncIndexByCodeIndex(thread_ctx->m_instruction_pointer) - 1), TOAST_ALL);
						return;
					}
					break;
#endif
				}
				if (send_to_self)
				{
					if (!ScriptEventTaxonomy::isDirectPacket(args, args_count))
					{
						std::unordered_map<int32_t, std::string> index_names{};
						auto res = ScriptEventTaxonomy::dissect(args, args_count, index_names, my_player_id);
						//#ifndef STAND_DEBUG
						//					if (res.types.empty()
						//						|| res.getCanonicalType() != FlowEvent::SE_INVALID
						//						)
						//#endif
						{
							if (hooks::apply_received_script_event_reactions(g_player, std::move(res), args, args_count, std::move(index_names)))
							{
								return;
							}
						}
					}
				}
			}
			return SEND_TU_SCRIPT_EVENT_ogfp(ctx);
		}
		__EXCEPTIONAL()
		{
		}
	}

	static rage::scrNativeHandler APPLY_FORCE_TO_ENTITY_ogfp;
	static void APPLY_FORCE_TO_ENTITY(rage::scrNativeCallContext& ctx)
	{
		// Ensure the native handler will be able to get a pointer as otherwise it will deref a nullptr in Online
		if (AbstractEntity::get(ctx.getArg<Entity>(0)).ensurePointer())
		{
			APPLY_FORCE_TO_ENTITY_ogfp(ctx);
		}
	}

	static rage::scrNativeHandler GET_CLOSEST_PED_ogfp;
	static void GET_CLOSEST_PED(rage::scrNativeCallContext& ctx)
	{
		// Force access violation now because we can't catch it if it happens in the arxan mess
		*ctx.getArg<Ped*>(6) = NULL_IN_SCRIPTING_LANGUAGE;
		GET_CLOSEST_PED_ogfp(ctx);
	}

	static rage::scrNativeHandler SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp;
	static void SET_ENTITY_AS_NO_LONGER_NEEDED(rage::scrNativeCallContext& ctx)
	{
		// Force access violation now because we can't catch it if it happens in the arxan mess
		volatile auto ent = *ctx.getArg<Entity*>(0);
		SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp(ctx);
	}

	static rage::scrNativeHandler GET_PED_HEAD_BLEND_DATA_ogfp;
	static void GET_PED_HEAD_BLEND_DATA(rage::scrNativeCallContext& ctx)
	{
		// Force access violation now because we can't catch it if it happens in the arxan mess
		volatile auto outHeadBlend = *ctx.getArg<void**>(1);
		GET_PED_HEAD_BLEND_DATA_ogfp(ctx);
	}

	static rage::scrNativeHandler DELETE_ENTITY_ogfp;
	static void DELETE_ENTITY(rage::scrNativeCallContext& ctx)
	{
		// Force access violation now because we can't catch it if it happens in the arxan mess
		volatile auto ent = *ctx.getArg<Entity*>(0);
		DELETE_ENTITY_ogfp(ctx);
	}

	static rage::scrNativeHandler DELETE_PED_ogfp;
	static void DELETE_PED(rage::scrNativeCallContext& ctx)
	{
		// Force access violation now because we can't catch it if it happens in the arxan mess
		volatile auto ped = *ctx.getArg<Ped*>(0);
		DELETE_PED_ogfp(ctx);
	}

	static rage::scrNativeHandler DELETE_VEHICLE_ogfp;
	static void DELETE_VEHICLE(rage::scrNativeCallContext& ctx)
	{
		// Force access violation now because we can't catch it if it happens in the arxan mess
		volatile auto veh = *ctx.getArg<Vehicle*>(0);
		DELETE_VEHICLE_ogfp(ctx);
	}

	static void GET_ENTITY_PROOFS(rage::scrNativeCallContext& ctx)
	{
		// Reimplement it because access violations here don't crash
		if (auto pEntity = AbstractEntity::get(ctx.getArg<Entity>(0)).getPointer())
		{
			*ctx.getArg<int*>(1) = pEntity->m_nPhysicalFlags.bNotDamagedByBullets;
			*ctx.getArg<int*>(2) = pEntity->m_nPhysicalFlags.bNotDamagedByFlames;
			*ctx.getArg<int*>(3) = pEntity->m_nPhysicalFlags.bIgnoresExplosions;
			*ctx.getArg<int*>(4) = pEntity->m_nPhysicalFlags.bNotDamagedByCollisions;
			*ctx.getArg<int*>(5) = pEntity->m_nPhysicalFlags.bNotDamagedByMelee;
			*ctx.getArg<int*>(6) = pEntity->m_nPhysicalFlags.bNotDamagedBySteam;
			*ctx.getArg<int*>(7) = pEntity->m_bDontResetDamageFlagsOnCleanupMissionState;
			*ctx.getArg<int*>(8) = pEntity->m_nPhysicalFlags.bNotDamagedBySmoke;
		}
	}

	static void SET_BIT(rage::scrNativeCallContext& ctx)
	{
		// Reimplement it because access violations here don't crash
		int* addr = ctx.getArg<int*>(0);
		int idx = ctx.getArg<int>(1);
		if (idx >= 0 && idx <= 31)
		{
			*addr |= (1 << idx);
		}
	}

	static void CLEAR_BIT(rage::scrNativeCallContext& ctx)
	{
		// Reimplement it because access violations here don't crash
		int* addr = ctx.getArg<int*>(0);
		int idx = ctx.getArg<int>(1);
		if (idx >= 0 && idx <= 31)
		{
			*addr &= ~(1 << idx);
		}
	}

#ifdef STAND_DEV
	static void REMOTE_CHEATER_PLAYER_DETECTED(rage::scrNativeCallContext& ctx)
	{
		SOUP_ASSERT_UNREACHABLE;
	}
#endif

	void SharedNativeHooks::enable(enabler_t consumer)
	{
		consumer(0x71A6F836422FDD2B, &SEND_TU_SCRIPT_EVENT, &SEND_TU_SCRIPT_EVENT_ogfp);

		consumer(0xC5F68BE9613E2D18, &APPLY_FORCE_TO_ENTITY, &APPLY_FORCE_TO_ENTITY_ogfp);
		consumer(0xC33AB876A77F8164, &GET_CLOSEST_PED, &GET_CLOSEST_PED_ogfp);
		consumer(0xB736A491E64A32CF, &SET_ENTITY_AS_NO_LONGER_NEEDED, &SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp);
		consumer(0x2595DD4236549CE3, &SET_ENTITY_AS_NO_LONGER_NEEDED, nullptr); // SET_PED_AS_NO_LONGER_NEEDED
		consumer(0x629BFA74418D6239, &SET_ENTITY_AS_NO_LONGER_NEEDED, nullptr); // SET_VEHICLE_AS_NO_LONGER_NEEDED
		consumer(0x3AE22DEB5BA5A3E6, &SET_ENTITY_AS_NO_LONGER_NEEDED, nullptr); // SET_OBJECT_AS_NO_LONGER_NEEDED
		consumer(0x2746BD9D88C5C5D0, &GET_PED_HEAD_BLEND_DATA, &GET_PED_HEAD_BLEND_DATA_ogfp);
		consumer(0xAE3CBE5BF394C9C9, &DELETE_ENTITY, &DELETE_ENTITY_ogfp);
		consumer(0x9614299DCB53E54B, &DELETE_PED, &DELETE_PED_ogfp);
		consumer(0xEA386986E786A54F, &DELETE_VEHICLE, &DELETE_VEHICLE_ogfp);
		consumer(0xBE8CD9BE829BBEBF, &GET_ENTITY_PROOFS, nullptr);
		consumer(0x933D6A9EEC1BACD0, &SET_BIT, nullptr);
		consumer(0xE80492A9AC099A93, &CLEAR_BIT, nullptr);

#ifdef STAND_DEV
		consumer(0x472841A026D26D8B, &REMOTE_CHEATER_PLAYER_DETECTED, nullptr);
#endif
	}

	void SharedNativeHooks::disable(disabler_t consumer)
	{
		consumer(0x71A6F836422FDD2B, SEND_TU_SCRIPT_EVENT_ogfp);

		consumer(0xC5F68BE9613E2D18, APPLY_FORCE_TO_ENTITY_ogfp);
		consumer(0xC33AB876A77F8164, GET_CLOSEST_PED_ogfp);
		consumer(0xB736A491E64A32CF, SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp);
		consumer(0x2595DD4236549CE3, SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp); // SET_PED_AS_NO_LONGER_NEEDED
		consumer(0x629BFA74418D6239, SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp); // SET_VEHICLE_AS_NO_LONGER_NEEDED
		consumer(0x3AE22DEB5BA5A3E6, SET_ENTITY_AS_NO_LONGER_NEEDED_ogfp); // SET_OBJECT_AS_NO_LONGER_NEEDED
		consumer(0x2746BD9D88C5C5D0, GET_PED_HEAD_BLEND_DATA_ogfp);
		consumer(0xAE3CBE5BF394C9C9, DELETE_ENTITY_ogfp);
		consumer(0x9614299DCB53E54B, DELETE_PED_ogfp);
		consumer(0xEA386986E786A54F, DELETE_VEHICLE_ogfp);
		consumer(0xBE8CD9BE829BBEBF, NativeTableHooks::og(0xBE8CD9BE829BBEBF));
		consumer(0x933D6A9EEC1BACD0, NativeTableHooks::og(0x933D6A9EEC1BACD0));
		consumer(0xE80492A9AC099A93, NativeTableHooks::og(0xE80492A9AC099A93));

#ifdef STAND_DEV
		consumer(0x472841A026D26D8B, NativeTableHooks::og(0x472841A026D26D8B));
#endif
	}
}
