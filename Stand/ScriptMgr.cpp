#include "ScriptMgr.hpp"

#include <soup/Bytepatch.hpp>

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "atArray.hpp"
#include "CommandToggleRegularSp.hpp"
#include "CustomDlcMgr.hpp"
#include "ExecCtx.hpp"
#include "Exceptional.hpp"
#include "FiberPool.hpp"
#include "atStringHash.hpp"
#include "main.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "ScriptGlobal.hpp"
#include "script_thread.hpp"
#include "scrNativeCallContext.hpp"
#include "scrNativeRegistrationTable.hpp"
#include "SharedNativeHooks.hpp"
#include "TickMgr.hpp"
#include "tlsContext.hpp"
#include "Tunables.hpp"
#include "UI3DDrawManager.hpp"
#include "Util.hpp"

#define USE_CROSSMAP true

#if USE_CROSSMAP
	#define CONVERT_CROSSMAP false
	#define LOG_DUPLICATE_ENTRYPOINTS false

	//static_assert(STAND_DEBUG);

	#include "crossmap.hpp"
	#if CONVERT_CROSSMAP || LOG_DUPLICATE_ENTRYPOINTS
		#include "FileLogger.hpp"
	#endif
#else
	#include "indirect_crossmap.hpp"
#endif

namespace Stand
{
	void ScriptMgr::addScript(HMODULE hmod, std::unique_ptr<Script>&& scr)
	{
		scripts.emplace(std::move(hmod), std::move(scr));
	}

	HMODULE ScriptMgr::getScriptModule(const Script* scr)
	{
		auto i = scripts.begin();
		while (i != scripts.end())
		{
			auto j = i->second.begin();
			while (j != i->second.end())
			{
				if (j->get() == scr)
				{
					return i->first;
				}
				j++;
			}
			i++;
		}
		return nullptr;
	}

	void ScriptMgr::removeScripts(HMODULE hmod)
	{
		if (auto e = scripts.find(hmod); e != scripts.end())
		{
			for (const auto& scr : e->second)
			{
				scr->stop();
			}
		}
	}

	void ScriptMgr::removeScript(script_func_t func)
	{
		for (const auto& mod : scripts)
		{
			for (const auto& scr : mod.second)
			{
				if (scr->func == func)
				{
					scr->stop();
					return;
				}
			}
		}
	}

	size_t ScriptMgr::getNumScripts(HMODULE hmod) const
	{
		return scripts.at(hmod).size();
	}

	void ScriptMgr::tick()
	{
		SOUP_IF_UNLIKELY (script_thread != _Thrd_id())
		{
			SOUP_ASSERT(script_thread == 0);
			script_thread = _Thrd_id();
			if (!IsThreadAFiber())
			{
				ConvertThreadToFiber(nullptr);
			}
		}
		FiberPool::onTick();
		Script::ret_fiber = GetCurrentFiber();
		for (auto i = scripts.begin(); i != scripts.end(); )
		{
			for (auto j = i->second.begin(); j != i->second.end(); )
			{
				bool ok = false;
				__try
				{
					ok = (*j)->tick();
				}
				__EXCEPTIONAL()
				{
				}
				if (ok)
				{
					++j;
				}
				else
				{
					j = i->second.erase(j);
					if (i->second.empty())
					{
						i = scripts.erase(i);
						goto _continue_2;
					}
				}
			}
			++i;
		_continue_2:;
		}
	}

	static void IS_BIT_SET(rage::scrNativeCallContext& ctx)
	{
		ctx.setReturnValue<BOOL>((*ctx.getArg<int*>(0) >> ctx.getArg<int>(1)) & 1);
	}

	static void IS_PLAYER_FREE_AIMING(rage::scrNativeCallContext& ctx)
	{
		ctx.setReturnValue<BOOL>(AbstractPlayer(ctx.getArg<Player>(0)).getPed().isAiming());
	}

	static void SET_PLAYER_INVINCIBLE(rage::scrNativeCallContext& ctx)
	{
		if (ctx.getArg<BOOL>(1))
		{
			g_player_ped.godmodeEnable(false);
		}
		else
		{
			g_player_ped.godmodeDisable();
		}
	}

	static void SET_EXPLOSIVE_AMMO_THIS_FRAME(rage::scrNativeCallContext& ctx)
	{
		TickMgr::expammo.this_tick = true;
	}

	static void SET_SUPER_JUMP_THIS_FRAME(rage::scrNativeCallContext& ctx)
	{
		TickMgr::super_jump.this_tick = true;
	}

	static void invokeSpawnNative(rage::scrNativeCallContext& ctx, rage::scrNativeHandler ogfp)
	{
		// This model_spawn_bypass causes CTheScripts::GetCurrentGtaScriptHandlerNetwork to always return the script handler,
		// although it's not really needed since we go to hijack scripts until it works.
		soup::Bytepatch patch;
		if (pointers::model_spawn_bypass != nullptr)
		{
			patch.initPatchNOP(pointers::model_spawn_bypass, 2);
		}

		if (ExecCtx::get().isScript()
			&& g_tunables.getBool(TUNAHASH("spawn.try_same_thread"))
			)
		{
			ogfp(ctx);
			SOUP_IF_LIKELY (ctx.getReturnValue<int>() != 0)
			{
				return;
			}
		}

		std::vector<GtaThread*> script_threads{};
		for (const auto& thread : *pointers::script_threads)
		{
			if (thread && thread->m_context.m_thread_id)
			{
				script_threads.emplace_back(thread);
			}
		}
		if (g_tunables.getBool(TUNAHASH("spawn.randomise_order")))
		{
			std::default_random_engine rng{};
			std::shuffle(script_threads.begin(), script_threads.end(), rng);
		}
		for (const auto& thread : script_threads)
		{
			thread->spoofAs([ctx{ &ctx }, ogfp]
			{
				ogfp(*ctx);
			});
			if (ctx.getReturnValue<int>() != 0)
			{
				break;
			}
		}
	}

	static void invokeSpawnObjectNative(rage::scrNativeCallContext& ctx, rage::scrNativeHandler ogfp)
	{
		soup::Bytepatch spawn_pseudo_object_patch{};
		if (pointers::pseudo_object_check != nullptr)
		{
			spawn_pseudo_object_patch.initPatchNOP(pointers::pseudo_object_check, 24);
		}
		invokeSpawnNative(ctx, ogfp);
	}

	static rage::scrNativeHandler CREATE_VEHICLE_ogfp;
	static void CREATE_VEHICLE(rage::scrNativeCallContext& ctx)
	{
		if (!CommandToggleRegularSp::checkPermissions()
			&& CustomDlcMgr::isCustomModel(ctx.getArg<Hash>(0))
			)
		{
			ctx.setReturnValue(0);
			return;
		}

		invokeSpawnNative(ctx, CREATE_VEHICLE_ogfp);

		const auto veh = ctx.getReturnValue<Vehicle>();
		SOUP_IF_LIKELY (veh != 0)
		{
			if (*pointers::is_session_started)
			{
				VEHICLE::SET_VEHICLE_IS_STOLEN(veh, FALSE);
			}
			else
			{
				*ScriptGlobal(GLOBAL_SP_DESPAWN_BYPASS).as<BOOL*>() = true;
			}

			// For some reason, spawned vehicles usually have seed 41, which produces the infamous 46EEK572.
			AbstractEntity::get(veh).randomiseSeedAndUpdatePlate();
		}
	}

	static rage::scrNativeHandler CREATE_OBJECT_ogfp;
	static void CREATE_OBJECT(rage::scrNativeCallContext& ctx)
	{
		invokeSpawnObjectNative(ctx, CREATE_OBJECT_ogfp);
	}

	static rage::scrNativeHandler CREATE_OBJECT_NO_OFFSET_ogfp;
	static void CREATE_OBJECT_NO_OFFSET(rage::scrNativeCallContext& ctx)
	{
		invokeSpawnObjectNative(ctx, CREATE_OBJECT_NO_OFFSET_ogfp);
	}

#define DECL_SPAWN_NATIVE(name) static rage::scrNativeHandler name ## _ogfp; \
	static void name(rage::scrNativeCallContext& ctx) \
	{ \
		invokeSpawnNative(ctx, name ## _ogfp); \
	}

	DECL_SPAWN_NATIVE(CREATE_PED);
	DECL_SPAWN_NATIVE(CREATE_PED_INSIDE_VEHICLE);
	DECL_SPAWN_NATIVE(CREATE_RANDOM_PED);
	DECL_SPAWN_NATIVE(CREATE_RANDOM_PED_AS_DRIVER);

	static rage::scrNativeHandler UI3DSCENE_ASSIGN_PED_TO_SLOT_ogfp;
	static void UI3DSCENE_ASSIGN_PED_TO_SLOT(rage::scrNativeCallContext& ctx)
	{
		const int slot = ctx.getArg<int>(2);

		UI3DSCENE_ASSIGN_PED_TO_SLOT_ogfp(ctx);

		if (pointers::ui3d_draw_mgr
			&& slot >= 0 && slot < UI_MAX_SCENE_PRESET_ELEMENTS
			)
		{
			(*pointers::ui3d_draw_mgr)->patched_data[slot].alpha = 1.0f;
		}
	}

	static rage::scrNativeHandler NETWORK_REQUEST_CONTROL_OF_ENTITY_ogfp;
	static void NETWORK_REQUEST_CONTROL_OF_ENTITY(rage::scrNativeCallContext& ctx)
	{
		soup::Bytepatch spectating_patch;
		if (pointers::request_control_spectating_check != nullptr)
		{
			spectating_patch.initPatchNOP(pointers::request_control_spectating_check, 9);
		}

		NETWORK_REQUEST_CONTROL_OF_ENTITY_ogfp(ctx);
	}

	static void COPY_SCRIPT_STRUCT(rage::scrNativeCallContext& ctx)
	{
		// There is no reason for anyone to be using this.
		// Safety risk because it could be used by Lua scripts to bypass policies in memory.write functions.
		SOUP_ASSERT_UNREACHABLE;
	}

	void ScriptMgr::initEntrypointMaps()
	{
#if USE_CROSSMAP
		// From crossmap
		std::unordered_set<rage::scrNativeHash> natives_mapped_cur_hashes{};
		for (const auto& mapping : g_crossmap)
		{
			auto og_hash = (mapping.first ^ MAGIC_CROSSMAP);
			auto current_hash = (mapping.second ^ MAGIC_CROSSMAP);
#if LOG_DUPLICATE_ENTRYPOINTS
			auto entrypoint = pointers::native_registration_table->getHandler(current_hash);
			if (reverse_entrypoint_map.contains(entrypoint))
			{
				if (reverse_entrypoint_map.at(entrypoint).size() == 1)
				{
					g_logger.log(fmt::format("{:X}", reverse_entrypoint_map.at(entrypoint).at(0)));
				}
				g_logger.log(fmt::format("{:X}", og_hash));
			}
#endif
			mapEntrypoint(og_hash, current_hash);
			natives_mapped_cur_hashes.emplace(current_hash);
		}

		// Add any natives missing in crossmap
		for (const auto& native : pointers::native_registration_table->getNatives())
		{
			if (!natives_mapped_cur_hashes.contains(native.first))
			{
				mapEntrypoint(native.first, native.second);
			}
		}

		// Redirect detected natives where alternative with same parameters is available
		constexpr std::pair<rage::scrNativeHash, rage::scrNativeHash> native_redirects[] = {
			{0x43A66C31C68491C0, 0x50FAC3A3E030A6E1}, // PLAYER::GET_PLAYER_PED — Alternative: PLAYER::GET_PLAYER_PED_SCRIPT_INDEX
		};
		for (const auto& native_redirect : native_redirects)
		{
			auto i = handler_map.find(native_redirect.first);
			if (i != handler_map.end())
			{
				i->second = handler_map.at(native_redirect.second);
			}
		}

#if CONVERT_CROSSMAP
		for (const auto& entry : entrypoint_map)
		{
			auto offset = uintptr_t(entry.second) - g_gta_module.range.base.as<uintptr_t>();
			if (offset % 4 != 0)
			{
				Util::toast("Not 4 byte aligned", TOAST_ALL);
			}
			g_logger.log(fmt::format("0x{:x}, 0x{:x}", entry.first, offset / 4));
		}
#endif
#else
		uint32_t* offset_ptr = &entrypoint_offsets[0];
		for (auto& og_hash : og_native_hashes)
		{
			mapEntrypoint(og_hash, reinterpret_cast<rage::scrNativeHandler>(g_gta_module.base().as<uintptr_t>() + ((*offset_ptr) * 4)));
			og_hash = 0;
			*offset_ptr = 0;
			++offset_ptr;
		}
#endif

		call_map = handler_map;

		// Redirect removed natives
		mapDetour(0xA921AA820C25702F, &IS_BIT_SET);

		// Redirect detected natives
		mapDetour(0x2E397FD2ECD37C87, &IS_PLAYER_FREE_AIMING);
		mapDetour(0x239528EACDC3E7DE, &SET_PLAYER_INVINCIBLE);
		mapDetour(0xA66C71C98D5F2CFB, &SET_EXPLOSIVE_AMMO_THIS_FRAME);
		mapDetour(0x57FFF03E423A4C0B, &SET_SUPER_JUMP_THIS_FRAME);
		/* Other detected natives:
		* IS_PLAYER_FREE_AIMING_AT_ENTITY
		* IS_PED_SHOOTING
		* SET_PLAYER_WANTED_LEVEL
		* SET_PLAYER_WANTED_LEVEL_NOW
		*/

		// Improve natives
		mapDetour(0xAF35D0D2583051B0, &CREATE_VEHICLE, &CREATE_VEHICLE_ogfp);
		mapDetour(0xD49F9B0955C367DE, &CREATE_PED, &CREATE_PED_ogfp);
		mapDetour(0x7DD959874C1FD534, &CREATE_PED_INSIDE_VEHICLE, &CREATE_PED_INSIDE_VEHICLE_ogfp);
		mapDetour(0x509D5878EB39E842, &CREATE_OBJECT, &CREATE_OBJECT_ogfp);
		mapDetour(0x9A294B2138ABB884, &CREATE_OBJECT_NO_OFFSET, &CREATE_OBJECT_NO_OFFSET_ogfp);
		mapDetour(0xB4AC7D0CF06BFE8F, &CREATE_RANDOM_PED, &CREATE_RANDOM_PED_ogfp);
		mapDetour(0x9B62392B474F44A0, &CREATE_RANDOM_PED_AS_DRIVER, &CREATE_RANDOM_PED_AS_DRIVER_ogfp);
		mapDetour(0x98C4FE6EC34154CA, &UI3DSCENE_ASSIGN_PED_TO_SLOT, &UI3DSCENE_ASSIGN_PED_TO_SLOT_ogfp);
		mapDetour(0xB69317BF5E782347, &NETWORK_REQUEST_CONTROL_OF_ENTITY, &NETWORK_REQUEST_CONTROL_OF_ENTITY_ogfp);
		mapDetour(0x213AEB2B90CBA7AC, &COPY_SCRIPT_STRUCT);

		SharedNativeHooks::enable([](XoredNativeHash target, rage::scrNativeHandler detour, rage::scrNativeHandler* pOriginal)
		{
			g_script_mgr.mapDetour(target.getHash(), detour, pOriginal);
		});
	}

	void ScriptMgr::mapEntrypoint(rage::scrNativeHash og_hash, rage::scrNativeHash current_hash)
	{
		if (auto handler = pointers::native_registration_table->getHandler(current_hash))
		{
			return mapEntrypoint(og_hash, handler);
		}
	}

	void ScriptMgr::mapEntrypoint(rage::scrNativeHash og_hash, rage::scrNativeHandler entrypoint)
	{
		handler_map.emplace(og_hash, entrypoint);
		mapEntrypointReverse(entrypoint, og_hash);
	}

	void ScriptMgr::mapEntrypointReverse(rage::scrNativeHandler entrypoint, rage::scrNativeHash og_hash)
	{
		reverse_entrypoint_map.emplace(std::move(entrypoint), std::move(og_hash));
	}

	void ScriptMgr::mapDetour(rage::scrNativeHash og_hash, rage::scrNativeHandler replacement, rage::scrNativeHandler* ogfpp)
	{
#if USE_ENTRYPOINT_DETOURS
		auto i = call_map.find(og_hash);
		if (i == call_map.end())
		{
			return;
		}
		if (ogfpp != nullptr)
		{
			*ogfpp = i->second;
		}
		i->second = replacement;
		mapEntrypointReverse(replacement, og_hash);
#endif
	}

	void ScriptMgr::deinit()
	{
		handler_map.clear();
		call_map.clear();
		reverse_entrypoint_map.clear();
		scripts.clear();
	}
}
