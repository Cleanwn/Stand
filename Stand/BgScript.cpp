#include "BgScript.hpp"

#include "CommandLuaScript.hpp"

#include <lualib.h>
#include <lstate.h>

#include <soup/aes.hpp>
#include <soup/base64.hpp>

#include "bin.hpp"
#include "Exceptional.hpp"
#include "Spinlock.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	static Spinlock mtx;
	static lua_State* g_L = nullptr;
	static LuaScriptData script_data;

	// BgScript introduced in 109.5 with cw, rs, rs2, & rs3
	void BgScript::boot(const std::string& code)
	{
		auto L = luaL_newstate();
		L->l_G->user_data = &script_data;
		lua_atpanic(L, &luaS_panic_handler);
		luaL_openlibs(L);

		script_data.nilptr = true;

		luaS_openMenu(L); // since 110
		luaS_openEntities(L); // since 24.8.6
		luaS_openUtil(L); // since 113.3
		luaS_openFilesystem(L); // since 24.8.5
		luaS_register_memory(L); // since 110
		luaS_register_native_invoker(L); // since 113.3
		luaS_openSoupLuaBindings(L); // since 110

		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			auto haystack = StringUtils::utf8_to_utf16(pluto_checkstring(L, 1));
			auto needle = StringUtils::utf8_to_utf16(pluto_checkstring(L, 2));
			lua_pushboolean(L, StringUtils::containsWord(haystack, needle));
			return 1;
		});
		lua_setglobal(L, "cw");

		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			auto wstr = StringUtils::utf8_to_utf16(pluto_checkstring(L, 1));
			wstr = StringUtils::removeSpaces(wstr);
			pluto_pushstring(L, StringUtils::utf16_to_utf8(wstr));
			return 1;
		});
		lua_setglobal(L, "rs");

		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			auto wstr = StringUtils::utf8_to_utf16(pluto_checkstring(L, 1));
			wstr = StringUtils::removeSpacesAndPunctuation(wstr);
			pluto_pushstring(L, StringUtils::utf16_to_utf8(wstr));
			return 1;
		});
		lua_setglobal(L, "rs2");

		lua_pushcfunction(L, [](lua_State* L) -> int
		{
			auto wstr = StringUtils::utf8_to_utf16(pluto_checkstring(L, 1));
			StringUtils::removeSpacesAroundPunctuation(wstr);
			pluto_pushstring(L, StringUtils::utf16_to_utf8(wstr));
			return 1;
		});
		lua_setglobal(L, "rs3");

		luaL_loadbuffer(L, code.data(), code.size(), nullptr);
		lua_pcall(L, 0, 0, 0);

		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_close(g_L);
		}
		g_L = L;
		EXCEPTIONAL_UNLOCK(mtx)
	}

	void BgScript::bootFromBase64(const std::string& data)
	{
		auto code = soup::base64::decode(data);
		//SOUP_ASSERT((code.size() % 16) == 0);
		soup::aes::ecbDecrypt(
			(uint8_t*)code.data(), code.size(),
			(const uint8_t*)"\x5f\x4f\x3b\x45\x0f\x25\x60\x63\x26\x59\x3b\xdd\xc3\x65\x4b\x63", 16
		);
		//soup::aes::pkcs7Unpad(code);
		boot(code);
	}

	void BgScript::bootFromBuiltin()
	{
		bootFromBase64(bin::bgscriptBuiltin);
	}

	void BgScript::shutdown()
	{
		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_close(g_L);
			g_L = nullptr;
		}
		EXCEPTIONAL_UNLOCK(mtx)
	}

	bool BgScript::hasFunction(const char* func) noexcept
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_getglobal(g_L, func);
			ret = (lua_type(g_L, -1) == LUA_TFUNCTION);
			lua_pop(g_L, 1);
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	void BgScript::invokeOptionalHook(const char* func) noexcept
	{
		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_getglobal(g_L, func);
			if (lua_type(g_L, -1) == LUA_TFUNCTION)
			{
				lua_call(g_L, 0, 0);
			}
			else
			{
				lua_pop(g_L, 1);
			}
		}
		EXCEPTIONAL_UNLOCK(mtx)
	}

	bool BgScript::query(const char* func, int64_t arg) noexcept
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_getglobal(g_L, func);
			lua_pushinteger(g_L, arg);
			lua_call(g_L, 1, 1);
			ret = lua_toboolean(g_L, -1);
			lua_pop(g_L, 1);
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	bool BgScript::query(const char* func, const std::string& arg) noexcept
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_getglobal(g_L, func);
			pluto_pushstring(g_L, arg);
			lua_call(g_L, 1, 1);
			ret = lua_toboolean(g_L, -1);
			lua_pop(g_L, 1);
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}

	bool BgScript::query(const char* func, int64_t a1, const std::string& a2, const std::string& a3) noexcept
	{
		bool ret = false;
		EXCEPTIONAL_LOCK(mtx)
		if (g_L != nullptr)
		{
			lua_getglobal(g_L, func);
			lua_pushinteger(g_L, a1);
			pluto_pushstring(g_L, a2);
			pluto_pushstring(g_L, a3);
			lua_call(g_L, 3, 1);
			ret = lua_toboolean(g_L, -1);
			lua_pop(g_L, 1);
		}
		EXCEPTIONAL_UNLOCK(mtx)
		return ret;
	}
}
