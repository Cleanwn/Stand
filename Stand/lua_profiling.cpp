#include "CommandLuaScript.hpp"
#include "lua_commons.hpp"

#include "TimedCall.hpp"

namespace Stand
{
	static int lua_profiling_once(lua_State* L)
	{
		luaL_checktype(L, 2, LUA_TFUNCTION);
		TimedCall::once(luaL_checkstring(L, 1), [L]
		{
			lua_pushvalue(L, 2);
			lua_call(L, 0, 0);
		});
		return 0;
	}

	static int lua_profiling_tick(lua_State* L)
	{
		luaL_checktype(L, 2, LUA_TFUNCTION);
		TimedCall::tick(luaL_checkstring(L, 1), [L]
		{
			lua_pushvalue(L, 2);
			lua_call(L, 0, 0);
		});
		return 0;
	}

	void luaS_register_profiling(lua_State* L)
	{
		LIB(profiling,
			LIB_FUNC(profiling, once)
			LIB_FUNC(profiling, tick)
		)
	}
}
