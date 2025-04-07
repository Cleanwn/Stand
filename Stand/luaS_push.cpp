#include "luaS_push.hpp"

#include <lapi.h>
#include <lstring.h>

#include <soup/unicode.hpp>

#include "CommandLuaScript.hpp"
#include "Label.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	void luaS_push(lua_State* L, const char* str)
	{
		lua_pushstring(L, str);
	}

	void luaS_push(lua_State* L, const std::string& str)
	{
		lua_pushlstring(L, str.data(), str.size());
	}

	void luaS_push(lua_State* L, const std::wstring& str)
	{
		luaS_push(L, StringUtils::utf16_to_utf8(str));
	}

	void luaS_push(lua_State* L, const std::u32string& str)
	{
		luaS_push(L, soup::unicode::utf32_to_utf8(str));
	}

#undef G

	void luaS_push(lua_State* L, bool v)
	{
		lua_pushboolean(L, v);
	}

	void luaS_push(lua_State* L, int8_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, uint8_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, int16_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, uint16_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, int32_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, uint32_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, int64_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, uint64_t v)
	{
		lua_pushinteger(L, v);
	}

	void luaS_push(lua_State* L, void* v)
	{
		lua_pushinteger(L, reinterpret_cast<lua_Integer>(v));
	}

	void luaS_push(lua_State* L, float v)
	{
		lua_pushnumber(L, v);
	}

	void luaS_push(lua_State* L, double v)
	{
		lua_pushnumber(L, v);
	}

	template <typename V3>
	static void luaS_pushV3(lua_State* L, const V3& vec)
	{
		auto v = luaS_newV3(L);
		v->x = vec.x;
		v->y = vec.y;
		v->z = vec.z;
	}

	void luaS_push(lua_State* L, const v3& vec)
	{
		luaS_pushV3(L, vec);
	}

	void luaS_push(lua_State* L, const rage::scrVector3& vec)
	{
		luaS_pushV3(L, vec);
	}

	void luaS_push(lua_State* L, const DirectX::SimpleMath::Color& colour)
	{
		lua_newtable(L);
		lua_pushstring(L, "r");
		lua_pushnumber(L, colour.R());
		lua_settable(L, -3);
		lua_pushstring(L, "g");
		lua_pushnumber(L, colour.G());
		lua_settable(L, -3);
		lua_pushstring(L, "b");
		lua_pushnumber(L, colour.B());
		lua_settable(L, -3);
		lua_pushstring(L, "a");
		lua_pushnumber(L, colour.A());
		lua_settable(L, -3);
	}

	void luaS_push(lua_State* L, const std::unordered_map<std::string, std::string>& data)
	{
		lua_newtable(L);
		for (const auto& entry : data)
		{
			luaS_push(L, entry.first);
			luaS_push(L, entry.second);
			lua_settable(L, -3);
		}
	}

	void luaS_push(lua_State* L, const std::vector<std::string>& data)
	{
		lua_newtable(L);
		size_t i = 1;
		for (const auto& entry : data)
		{
			luaS_push(L, i++);
			luaS_push(L, entry);
			lua_settable(L, -3);
		}
	}

	void luaS_push(lua_State* L, const std::vector<std::wstring>& data)
	{
		lua_newtable(L);
		size_t i = 1;
		for (const auto& entry : data)
		{
			luaS_push(L, i++);
			luaS_push(L, StringUtils::utf16_to_utf8(entry));
			lua_settable(L, -3);
		}
	}

	void luaS_push(lua_State* L, const Label& label)
	{
		if (label.isLiteral())
		{
			luaS_push(L, label.literal_str);
		}
		else
		{
			luaS_push(L, label.getLocalisationHash());
		}
	}

	void luaS_pushPtrImpl(lua_State* L, void* v)
	{
		lua_pushinteger(L, reinterpret_cast<lua_Integer>(v));
	}

	void luaS_pushPtrNilableImpl(lua_State* L, void* v)
	{
		SOUP_IF_UNLIKELY (v == nullptr)
		{
			/*if (!luaS_scriptData(L)->nilptr.has_value())
			{
				luaS_deprecatedCustom(L, "Using this function before first calling util.set_nullptr_preference is deprecated");
			}*/
			if (luaS_scriptData(L)->isNilptrOptedIn())
			{
				lua_pushnil(L);
				return;
			}
		}
		luaS_pushPtrImpl(L, v);
	}
}
