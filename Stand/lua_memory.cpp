#include "CommandLuaScript.hpp"
#include "lua_commons.hpp"

#include "LuaConfig.hpp"

#include <soup/Module.hpp>
#include <soup/Pattern.hpp>
#include <soup/Pointer.hpp>
#include <soup/Thread.hpp>
#include "ExecCtx.hpp"
#include "main.hpp"
#include "pointers.hpp"
#include "Script.hpp"
#include "ScriptGlobal.hpp"
#include "ScriptLocal.hpp"
#include "script_program.hpp"
#include "script_thread.hpp"
#include "Tunables.hpp"
#include "TunablesParser.hpp"

namespace Stand
{
	static int lua_memory_script_global(lua_State* L)
	{
		if (!ExecCtx::get().isScript())
		{
			if (LuaConfig::notify_bad_practice->m_on)
			{
				luaS_thisptr(L)->devNoticeNoCheck("memory.script_global should not be used within OS threads");
			}
		}
		beginBadSgTamperCodeThatWillLandUsInHell();
		luaS_pushPtrNilable(L, ScriptGlobal((uint32_t)luaL_checkinteger(L, 1)).as<void*>());
		endBadSgTamperCodeThatWillLandUsInHell();
		return 1;
	}

	static int lua_memory_script_local(lua_State* L)
	{
		auto thread = GtaThread::fromHash(luaS_checkHash(L, 1));
		if (thread == nullptr)
		{
			luaS_pushPtrNilable<void*>(L, nullptr);
		}
		else
		{
			luaS_pushPtrNilable(L, ScriptLocal(thread, (uintptr_t)luaL_checkinteger(L, 2)).as<void*>());
		}
		return 1;
	}

	static int lua_memory_read_byte(lua_State* L);
	static int lua_memory_read_ubyte(lua_State* L);
	static int lua_memory_read_short(lua_State* L);
	static int lua_memory_read_ushort(lua_State* L);
	static int lua_memory_read_int(lua_State* L);
	static int lua_memory_read_uint(lua_State* L);
	static int lua_memory_read_long(lua_State* L);
	static int lua_memory_read_float(lua_State* L);
	static int lua_memory_read_string(lua_State* L);
	static int lua_memory_read_vector3(lua_State* L);
	static int lua_memory_read_binary_string(lua_State* L);
	static int lua_memory_write_byte(lua_State* L);
	static int lua_memory_write_ubyte(lua_State* L);
	static int lua_memory_write_short(lua_State* L);
	static int lua_memory_write_ushort(lua_State* L);
	static int lua_memory_write_int(lua_State* L);
	static int lua_memory_write_uint(lua_State* L);
	static int lua_memory_write_long(lua_State* L);
	static int lua_memory_write_float(lua_State* L);
	static int lua_memory_write_string(lua_State* L);
	static int lua_memory_write_vector3(lua_State* L);
	static int lua_memory_write_binary_string(lua_State* L);

	static int lua_mem_index(lua_State* L)
	{
		switch (rage::atStringHash(luaL_checkstring(L, 2)))
		{
		case ATSTRINGHASH("readByte"): lua_pushcfunction(L, &lua_memory_read_byte); return 1;
		case ATSTRINGHASH("readUbyte"): lua_pushcfunction(L, &lua_memory_read_ubyte); return 1;
		case ATSTRINGHASH("readShort"): lua_pushcfunction(L, &lua_memory_read_short); return 1;
		case ATSTRINGHASH("readUshort"): lua_pushcfunction(L, &lua_memory_read_ushort); return 1;
		case ATSTRINGHASH("readInt"): lua_pushcfunction(L, &lua_memory_read_int); return 1;
		case ATSTRINGHASH("readUint"): lua_pushcfunction(L, &lua_memory_read_uint); return 1;
		case ATSTRINGHASH("readLong"): lua_pushcfunction(L, &lua_memory_read_long); return 1;
		case ATSTRINGHASH("readFloat"): lua_pushcfunction(L, &lua_memory_read_float); return 1;
		case ATSTRINGHASH("readString"): lua_pushcfunction(L, &lua_memory_read_string); return 1;
		case ATSTRINGHASH("readVector3"): lua_pushcfunction(L, &lua_memory_read_vector3); return 1;
		case ATSTRINGHASH("readBinaryString"): lua_pushcfunction(L, &lua_memory_read_binary_string); return 1;
		case ATSTRINGHASH("writeByte"): lua_pushcfunction(L, &lua_memory_write_byte); return 1;
		case ATSTRINGHASH("writeUbyte"): lua_pushcfunction(L, &lua_memory_write_ubyte); return 1;
		case ATSTRINGHASH("writeShort"): lua_pushcfunction(L, &lua_memory_write_short); return 1;
		case ATSTRINGHASH("writeUshort"): lua_pushcfunction(L, &lua_memory_write_ushort); return 1;
		case ATSTRINGHASH("writeInt"): lua_pushcfunction(L, &lua_memory_write_int); return 1;
		case ATSTRINGHASH("writeUint"): lua_pushcfunction(L, &lua_memory_write_uint); return 1;
		case ATSTRINGHASH("writeLong"): lua_pushcfunction(L, &lua_memory_write_long); return 1;
		case ATSTRINGHASH("writeFloat"): lua_pushcfunction(L, &lua_memory_write_float); return 1;
		case ATSTRINGHASH("writeString"): lua_pushcfunction(L, &lua_memory_write_string); return 1;
		case ATSTRINGHASH("writeVector3"): lua_pushcfunction(L, &lua_memory_write_vector3); return 1;
		case ATSTRINGHASH("writeBinaryString"): lua_pushcfunction(L, &lua_memory_write_binary_string); return 1;
		}
		return 0;
	}

	static int lua_memory_alloc(lua_State* L)
	{
		size_t size = (lua_gettop(L) >= 1 ? luaL_checkinteger(L, 1) : 24);
#if LUA_MEMORY_DEBUG
		allocated_ud.emplace_back(lua_newuserdata(L, LUA_MEMORY_DEBUG_SIZE), size);
#else
		lua_newuserdata(L, size);
#endif

		lua_newtable(L);
		luaS_push_mem_metafuncs(L);

		luaS_push(L, "__index");
		lua_pushcfunction(L, &lua_mem_index);
		lua_settable(L, -3);

#if LUA_MEMORY_DEBUG
		luaS_push(L, "__gc");
		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			void* ud = lua_touserdata(L, 1);
			for (auto i = allocated_ud.begin(); i != allocated_ud.end(); ++i)
			{
				if (i->first == ud)
				{
					allocated_ud.erase(i);
					break;
				}
			}
			return 0;
		});
		lua_settable(L, -3);
#endif

		lua_setmetatable(L, -2);

		return 1;
	}

	static int lua_memory_free(lua_State* L)
	{
		luaS_deprecated(L, "memory.free"); // since 0.83
		return 0;
	}

	static int lua_memory_scan(lua_State* L)
	{
		soup::Range r;
		soup::Pattern p;
		if (lua_gettop(L) >= 2)
		{
			luaS_tryCatch<void>(L, [L, &r]
			{
				const char* module_name = luaL_checkstring(L, 1);
				r = soup::Module(module_name).range;
				if (g_stand_dll_file_name == module_name)
				{
					r.size = g_stand_dll_range.size;
				}
			});
			size_t l;
			const char* s = luaL_checklstring(L, 2, &l);
			p = soup::Pattern(s, l);
		}
		else
		{
			r = g_gta_module.range;
			size_t l;
			const char* s = luaL_checklstring(L, 1, &l);
			p = soup::Pattern(s, l);
		}
		return luaS_returnPtrNilable(L, r.scan(p).as<void*>());
	}

	static int lua_memory_scan_script(lua_State* L)
	{
		if (auto prog = rage::scrProgram::fromHash(luaS_checkHash(L, 1)))
		{
			size_t l;
			const char* s = luaL_checklstring(L, 2, &l);
			return luaS_returnPtrNilable(L, prog->getCodePtr(prog->scan(soup::Pattern(s, l))));
		}
		return 0;
	}

	static int lua_memory_rip(lua_State* L)
	{
		auto res = soup::Pointer(luaL_checkinteger(L, 1)).rip().as<uint64_t>();
		if (LuaConfig::notify_bad_practice->m_on && res == (uint64_t)pointers::ped_factory)
		{
			luaS_notice(L, "memory.scan used as a low-speed alternative to entities.handle_to_pointer(players.user_ped())");
		}
		lua_pushinteger(L, res);
		return 1;
	}

	static int lua_memory_addrof(lua_State* L)
	{
		lua_pushlightuserdata(L, luaS_checkuserdata(L, 1));
		return 1;
	}

	template<typename T>
	inline T memory_read(lua_State* L)
	{
		return *(T*)luaS_checkPointer(L, 1);
	}

	static int lua_memory_read_byte(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<int8_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_ubyte(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<uint8_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_short(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<int16_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_ushort(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<uint16_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_int(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<int32_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_uint(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<uint32_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_long(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushinteger(L, memory_read<int64_t>(L));
			return 1;
		});
	}

	static int lua_memory_read_float(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushnumber(L, (lua_Number)memory_read<float>(L));
			return 1;
		});
	}

	static int lua_memory_read_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushstring(L, (const char*)luaS_checkPointer(L, 1));
			return 1;
		});
	}

	static int lua_memory_read_vector3(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_push(L, memory_read<Vector3>(L));
			return 1;
		});
	}

	static int lua_memory_read_binary_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushlstring(L, (const char*)luaS_checkPointer(L, 1), luaL_checkinteger(L, 2));
			return 1;
		});
	}

	static void check_write_access(lua_State* L, void* addr)
	{
		if (g_gta_module.range.contains(addr))
		{
			if (g_tunables.getBool(TUNAHASH("lua.memory.disallow_write_in_game_process")))
			{
				luaL_error(L, "Attempt to write to unwritable memory");
			}
			if (g_tunables.getBool(TUNAHASH("lua.memory.write_in_game_process_requires_warning")) && !luaS_thisptr(L)->showed_os_warning)
			{
				luaL_error(L, "Writing to this memory is not allowed without first calling util.i_really_need_manual_access_to_process_apis");
			}
		}
	}

	template <typename T>
	static void memory_write(lua_State* L, T value)
	{
		void* addr = luaS_checkPointer(L, 1);
		check_write_access(L, addr);
		*reinterpret_cast<T*>(addr) = value;
	}

	static void memory_write(lua_State* L, const void* data, size_t size)
	{
		void* addr = luaS_checkPointer(L, 1);
		check_write_access(L, addr);
		memcpy(addr, data, size);
	}

	static int lua_memory_write_byte(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (int8_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_ubyte(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (uint8_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_short(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (int16_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_ushort(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (uint16_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_int(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (int32_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_uint(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (uint32_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_long(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (int64_t)luaL_checkinteger(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_float(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, (float)luaL_checknumber(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			size_t size;
			const char* data = luaL_checklstring(L, 2, &size);
			size += 1; // null terminator
			memory_write(L, data, size);
			return 0;
		});
	}

	static int lua_memory_write_vector3(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			memory_write(L, luaS_checkV3<rage::scrVector3>(L, 2));
			return 0;
		});
	}

	static int lua_memory_write_binary_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			size_t size;
			auto data = luaL_checklstring(L, 2, &size);
			memory_write(L, data, size);
			return 0;
		});
	}

	static int lua_memory_get_name_of_this_module(lua_State* L)
	{
		luaS_push(L, g_stand_dll_file_name);
		return 1;
	}

	static int lua_memory_tunables_can_query(lua_State* L) // undocumented
	{
		luaS_push(L, TunablesParser::canQuery());
		return 1;
	}

	static int lua_memory_tunables_query_offset(lua_State* L) // undocumented
	{
		luaS_push(L, TunablesParser::queryOffset(luaS_checkHash(L, 1)));
		return 1;
	}

	void luaS_register_memory(lua_State* L)
	{
		LIB(memory,
			LIB_FUNC(memory, script_global)
			LIB_FUNC(memory, script_local)
			LIB_FUNC(memory, alloc)
			LIB_FUNC(memory, free)
			LIB_FUNC(memory, scan)
			LIB_FUNC(memory, scan_script)
			LIB_FUNC(memory, rip)
			LIB_FUNC(memory, addrof)
			LIB_FUNC(memory, read_byte)
			LIB_FUNC(memory, read_ubyte)
			LIB_FUNC(memory, read_short)
			LIB_FUNC(memory, read_ushort)
			LIB_FUNC(memory, read_int)
			LIB_FUNC(memory, read_uint)
			LIB_FUNC(memory, read_long)
			LIB_FUNC(memory, read_float)
			LIB_FUNC(memory, read_string)
			LIB_FUNC(memory, read_vector3)
			LIB_FUNC(memory, read_binary_string)
			LIB_FUNC(memory, write_byte)
			LIB_FUNC(memory, write_ubyte)
			LIB_FUNC(memory, write_short)
			LIB_FUNC(memory, write_ushort)
			LIB_FUNC(memory, write_int)
			LIB_FUNC(memory, write_uint)
			LIB_FUNC(memory, write_long)
			LIB_FUNC(memory, write_float)
			LIB_FUNC(memory, write_string)
			LIB_FUNC(memory, write_vector3)
			LIB_FUNC(memory, write_binary_string)
			LIB_FUNC(memory, get_name_of_this_module)
			LIB_FUNC(memory, tunables_can_query)
			LIB_FUNC(memory, tunables_query_offset)
		)
	}
}
