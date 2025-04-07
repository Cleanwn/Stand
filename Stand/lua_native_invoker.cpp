#include "CommandLuaScript.hpp"
#include "lua_commons.hpp"

namespace Stand
{
	static int lua_native_invoker_accept_bools_as_ints(lua_State* L) // undocumented
	{
		luaS_push(L, luaS_scriptData(L)->accept_bools_as_ints);
		if (lua_gettop(L) >= 1)
		{
			luaS_scriptData(L)->accept_bools_as_ints = luaS_checkboolean(L, 1);
		}
		return 1;
	}

	static int lua_native_invoker_c_style_int_to_bool_casts(lua_State* L) // undocumented
	{
		luaS_push(L, luaS_scriptData(L)->c_style_int_to_bool_casts);
		if (lua_gettop(L) >= 1)
		{
			luaS_scriptData(L)->c_style_int_to_bool_casts = luaS_checkboolean(L, 1);
		}
		return 1;
	}

	static int lua_native_invoker_accept_nils_as_strings(lua_State* L) // undocumented
	{
		luaS_push(L, luaS_scriptData(L)->accept_nils_as_strings);
		if (lua_gettop(L) >= 1)
		{
			luaS_scriptData(L)->accept_nils_as_strings = luaS_checkboolean(L, 1);
		}
		return 1;
	}

	static int lua_native_invoker_begin_call(lua_State* L)
	{
		auto data = luaS_scriptData(L);
		data->native_call_ctx.reset();
		data->vector3_cache.clear();
		return 0;
	}

	static int lua_native_invoker_push_arg_int(lua_State* L)
	{
		if (lua_gettop(L) != 1)
		{
			luaL_error(L, "native_invoker.push_arg_int expects 1 integer parameter");
		}
		luaS_scriptData(L)->native_call_ctx.pushArg((int)lua_tointeger(L, 1));
		return 0;
	}

	static void ctx_push_bool(NativeCallContext& ctx, lua_State* L, int i)
	{
		if (lua_type(L, i) == LUA_TNUMBER && luaS_scriptData(L)->c_style_int_to_bool_casts)
		{
			ctx.pushArg((BOOL)(bool)(int)lua_tointeger(L, i));
		}
		else
		{
			ctx.pushArg((BOOL)luaS_checkboolean(L, i));
		}
	}

	static int lua_native_invoker_push_arg_bool(lua_State* L)
	{
		ctx_push_bool(luaS_scriptData(L)->native_call_ctx, L, 1);
		return 0;
	}

	static int lua_native_invoker_push_arg_float(lua_State* L)
	{
		luaS_scriptData(L)->native_call_ctx.pushArg((float)luaL_checknumber(L, 1));
		return 0;
	}

	static void ctx_push_string(NativeCallContext& ctx, lua_State* L, int i)
	{
		if (lua_isinteger(L, i))
		{
			ctx.pushArg((void*)luaL_checkinteger(L, i));
		}
		else if (lua_isuserdata(L, i))
		{
			ctx.pushArg(lua_touserdata(L, i));
		}
		else if (lua_type(L, i) == LUA_TNIL && luaS_scriptData(L)->accept_nils_as_strings)
		{
			ctx.pushArg(nullptr);
		}
		else
		{
			ctx.pushArg(luaL_checkstring(L, i));
		}
	}

	static int lua_native_invoker_push_arg_string(lua_State* L)
	{
		ctx_push_string(luaS_scriptData(L)->native_call_ctx, L, 1);
		return 0;
	}

	static int lua_native_invoker_push_arg_vector3(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			auto data = luaS_scriptData(L);
			data->native_call_ctx.pushArg(&data->vector3_cache.emplace_front(luaS_checkV3<rage::scrVector3>(L, 1)));
			return 0;
		});
	}

	static void ctx_push_pointer(NativeCallContext& ctx, lua_State* L, int i)
	{
		if (lua_isuserdata(L, i))
		{
			ctx.pushArg(lua_touserdata(L, i));
		}
		else if (!lua_isinteger(L, i) && lua_isstring(L, i))
		{
			ctx.pushArg(lua_tolstring(L, i, nullptr));
		}
		else
		{
			ctx.pushArg((void*)luaL_checkinteger(L, i));
		}
	}

	static int lua_native_invoker_push_arg_pointer(lua_State* L)
	{
		ctx_push_pointer(luaS_scriptData(L)->native_call_ctx, L, 1);
		return 0;
	}

	static int lua_native_invoker_end_call(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_scriptData(L)->native_call_ctx.invoke(std::stoull(luaL_checkstring(L, 1), nullptr, 16));
			return 0;
		});
	}

	// natives-1660775568 and later
	static int lua_native_invoker_end_call_2(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			luaS_scriptData(L)->native_call_ctx.invoke(luaL_checkinteger(L, 1));
			return 0;
		});
	}

	static int lua_native_invoker_get_return_value_int(lua_State* L)
	{
		lua_pushinteger(L, luaS_scriptData(L)->native_call_ctx.getReturnValue<int>());
		return 1;
	}

	static int lua_native_invoker_get_return_value_bool(lua_State* L)
	{
		lua_pushboolean(L, luaS_scriptData(L)->native_call_ctx.getReturnValue<BOOL>());
		return 1;
	}

	static int lua_native_invoker_get_return_value_float(lua_State* L)
	{
		lua_pushnumber(L, luaS_scriptData(L)->native_call_ctx.getReturnValue<float>());
		return 1;
	}

	static int lua_native_invoker_get_return_value_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			lua_pushstring(L, luaS_scriptData(L)->native_call_ctx.getReturnValue<const char*>());
			return 1;
		});
	}

	static int lua_native_invoker_get_return_value_vector3(lua_State* L)
	{
		luaS_push(L, luaS_scriptData(L)->native_call_ctx.getReturnValue<Vector3>());
		return 1;
	}

	static int lua_native_invoker_get_return_value_pointer(lua_State* L)
	{
		lua_pushinteger(L, luaS_scriptData(L)->native_call_ctx.getReturnValue<long long>());
		return 1;
	}

	static void lua_native_invoker_uno_invoke(lua_State* L, NativeCallContext& ctx)
	{
		const int num_args = lua_gettop(L);
		for (int i = 2; i <= num_args; ++i)
		{
			switch (lua_type(L, i))
			{
			case LUA_TBOOLEAN:
				ctx.pushArg(lua_toboolean(L, i));
				break;

			case LUA_TNUMBER:
				if (lua_isinteger(L, i))
				{
					ctx.pushArg((int)luaL_checkinteger(L, i));
				}
				else
				{
					ctx.pushArg((float)luaL_checknumber(L, i));
				}
				break;

			case LUA_TSTRING:
				ctx.pushArg(luaL_checkstring(L, i));
				break;

			case LUA_TUSERDATA:
				if (luaS_isV3Instance(L, i))
				{
					ctx.pushArg(*reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, i)));
					break;
				}
				[[fallthrough]];
			case LUA_TLIGHTUSERDATA:
				ctx.pushArg(lua_touserdata(L, i));
				break;

			default:
				luaL_error(L, fmt::format("Argument {} is of an unsupported type", i).c_str());
				break;
			}
		}
		ctx.invoke(luaL_checkinteger(L, 1));
	}

	static int lua_native_invoker_uno_void(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			return 0;
		});
	}

	static int lua_native_invoker_uno_int(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			lua_pushinteger(L, ctx.getReturnValue<int>());
			return 1;
		});
	}

	static int lua_native_invoker_uno_bool(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			lua_pushboolean(L, ctx.getReturnValue<BOOL>());
			return 1;
		});
	}

	static int lua_native_invoker_uno_float(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			lua_pushnumber(L, ctx.getReturnValue<float>());
			return 1;
		});
	}

	static int lua_native_invoker_uno_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			luaS_push(L, ctx.getReturnValue<const char*>());
			return 1;
		});
	}

	static int lua_native_invoker_uno_vector3(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			luaS_push(L, ctx.getReturnValue<Vector3>());
			return 1;
		});
	}

	static int lua_native_invoker_uno_pointer(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_uno_invoke(L, ctx);
			lua_pushinteger(L, ctx.getReturnValue<long long>());
			return 1;
		});
	}

	static void lua_native_invoker_unified_invoke(lua_State* L, NativeCallContext& ctx)
	{
		auto num_args = lua_gettop(L);
		SOUP_IF_UNLIKELY (num_args < 2)
		{
			luaL_error(L, "Assertion failed");
		}
		const auto hash = luaL_checkinteger(L, num_args - 1);
		const char* types = luaL_checkstring(L, num_args - 0);
		num_args -= 2;
		for (int i = 1; i <= num_args; ++i)
		{
			//g_logger.log(fmt::format("Call to {:X}, processing arg {}, types = {}", hash, i, types));
			switch (*types++)
			{
			case 'i':
				if (lua_type(L, i) == LUA_TBOOLEAN && luaS_scriptData(L)->accept_bools_as_ints)
				{
					ctx.pushArg((BOOL)lua_toboolean(L, i));
				}
				else
				{
					ctx.pushArg((int)luaL_checkinteger(L, i));
				}
				break;

			case 'b':
				ctx_push_bool(ctx, L, i);
				break;

			case 'f':
				if (luaS_isV3Instance(L, i) && types[0] == 'f' && types[1] == 'f')
				{
					types += 2;
					num_args -= 2;
					ctx.pushArg(*reinterpret_cast<rage::scrVector3*>(lua_touserdata(L, i)));
				}
				else
				{
					ctx.pushArg((float)luaL_checknumber(L, i));
				}
				break;

			case 's':
				ctx_push_string(ctx, L, i);
				break;

			case 'v':
				ctx.pushArg(luaS_checkV3<rage::scrVector3>(L, i));
				break;

			case 'p':
				ctx_push_pointer(ctx, L, i);
				break;

			default:
				luaL_error(L, "Arguments mismatch");
			}
		}
		ctx.invoke(hash);
	}

	static int lua_native_invoker_unified_void(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			return 0;
		});
	}

	static int lua_native_invoker_unified_int(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			lua_pushinteger(L, ctx.getReturnValue<int>());
			return 1;
		});
	}

	static int lua_native_invoker_unified_bool(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			lua_pushboolean(L, ctx.getReturnValue<BOOL>());
			return 1;
		});
	}

	static int lua_native_invoker_unified_float(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			lua_pushnumber(L, ctx.getReturnValue<float>());
			return 1;
		});
	}

	static int lua_native_invoker_unified_string(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			luaS_push(L, ctx.getReturnValue<const char*>());
			return 1;
		});
	}

	static int lua_native_invoker_unified_vector3(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			luaS_push(L, ctx.getReturnValue<Vector3>());
			return 1;
		});
	}

	static int lua_native_invoker_unified_pointer(lua_State* L)
	{
		return luaS_tryCatch(L, [](lua_State* L)
		{
			NativeCallContext ctx{};
			lua_native_invoker_unified_invoke(L, ctx);
			lua_pushinteger(L, ctx.getReturnValue<long long>());
			return 1;
		});
	}

	void luaS_register_native_invoker(lua_State* L)
	{
		LIB(native_invoker,
			LIB_FUNC(native_invoker, accept_bools_as_ints)
			LIB_FUNC(native_invoker, c_style_int_to_bool_casts)
			LIB_FUNC(native_invoker, accept_nils_as_strings)
			LIB_FUNC(native_invoker, begin_call)
			LIB_FUNC(native_invoker, push_arg_int)
			LIB_FUNC(native_invoker, push_arg_bool)
			LIB_FUNC(native_invoker, push_arg_float)
			LIB_FUNC(native_invoker, push_arg_string)
			LIB_FUNC(native_invoker, push_arg_vector3)
			LIB_FUNC(native_invoker, push_arg_pointer)
			LIB_FUNC(native_invoker, end_call)
			LIB_FUNC(native_invoker, end_call_2)
			LIB_FUNC(native_invoker, get_return_value_int)
			LIB_FUNC(native_invoker, get_return_value_bool)
			LIB_FUNC(native_invoker, get_return_value_float)
			LIB_FUNC(native_invoker, get_return_value_string)
			LIB_FUNC(native_invoker, get_return_value_vector3)
			LIB_FUNC(native_invoker, get_return_value_pointer)
			LIB_FUNC(native_invoker, uno_void)
			LIB_FUNC(native_invoker, uno_int)
			LIB_FUNC(native_invoker, uno_bool)
			LIB_FUNC(native_invoker, uno_float)
			LIB_FUNC(native_invoker, uno_string)
			LIB_FUNC(native_invoker, uno_vector3)
			LIB_FUNC(native_invoker, uno_pointer)
			LIB_FUNC(native_invoker, unified_void)
			LIB_FUNC(native_invoker, unified_int)
			LIB_FUNC(native_invoker, unified_bool)
			LIB_FUNC(native_invoker, unified_float)
			LIB_FUNC(native_invoker, unified_string)
			LIB_FUNC(native_invoker, unified_vector3)
			LIB_FUNC(native_invoker, unified_pointer)
		)
	}
}
