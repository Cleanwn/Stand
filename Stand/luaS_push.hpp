#pragma once

#include <cstdint>

#include <lua.h>

#include "dx_common.hpp"
#include "fwddecl.hpp"
#include "vector.hpp"

namespace Stand
{
	extern void luaS_push(lua_State* L, const char* str);
	extern void luaS_push(lua_State* L, const std::string& str);
	extern void luaS_push(lua_State* L, const std::wstring& str);
	extern void luaS_push(lua_State* L, const std::u32string& str);
	extern void luaS_push(lua_State* L, bool v);
	extern void luaS_push(lua_State* L, int8_t v);
	extern void luaS_push(lua_State* L, uint8_t v);
	extern void luaS_push(lua_State* L, int16_t v);
	extern void luaS_push(lua_State* L, uint16_t v);
	extern void luaS_push(lua_State* L, int32_t v);
	extern void luaS_push(lua_State* L, uint32_t v);
	extern void luaS_push(lua_State* L, int64_t v);
	extern void luaS_push(lua_State* L, uint64_t v);
	extern void luaS_push(lua_State* L, void* v);
	extern void luaS_push(lua_State* L, float v);
	extern void luaS_push(lua_State* L, double v);
	extern void luaS_push(lua_State* L, const v3& vec);
	extern void luaS_push(lua_State* L, const rage::scrVector3& vec);
	extern void luaS_push(lua_State* L, const DirectX::SimpleMath::Color& colour);
	extern void luaS_push(lua_State* L, const std::unordered_map<std::string, std::string>& data);
	extern void luaS_push(lua_State* L, const std::vector<std::string>& data);
	extern void luaS_push(lua_State* L, const std::vector<std::wstring>& data);
	extern void luaS_push(lua_State* L, const Label& label);

	extern void luaS_pushPtrImpl(lua_State* L, void* v);

	template <typename T, SOUP_RESTRICT(std::is_pointer_v<T>)>
	inline void luaS_pushPtr(lua_State* L, T v)
	{
		luaS_pushPtrImpl(L, v);
	}

	extern void luaS_pushPtrNilableImpl(lua_State* L, void* v);

	template <typename T, SOUP_RESTRICT(std::is_pointer_v<T>)>
	inline void luaS_pushPtrNilable(lua_State* L, T v)
	{
		luaS_pushPtrNilableImpl(L, v);
	}

	template <typename T, SOUP_RESTRICT(std::is_pointer_v<T>)>
	[[nodiscard]] inline int luaS_returnPtrNilable(lua_State* L, T v)
	{
		luaS_pushPtrNilableImpl(L, v);
		return 1;
	}
}
