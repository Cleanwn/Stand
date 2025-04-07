#pragma once

#include <soup/ObfusString.hpp>

#include "PointerNames.hpp"

#define EXPOSE_INT_CONSTANT(constant) { \
	lua_pushinteger(L, constant); \
	soup::ObfusString key(#constant); \
	lua_setglobal(L, key.c_str()); \
}

#define LIB_FUNC(lib, func) LIB_FUNC_CUSTOM(func, &lua_ ## lib ## _ ## func)
#define LIB_FUNC_CUSTOM(name, func) \
{ \
	std::string func_name = soup::ObfusString( #name ).str(); \
	std::string fqn = lib_name_dot; \
	fqn.append(func_name); \
	luaS_push(L, std::move(func_name)); \
	lua_pushcfunction(L, func); \
	lua_settable(L, -3); \
	PointerNames::registerFunctionName((void*)(lua_CFunction)func, std::move(fqn)); \
}

#define LIB(lib, ...) \
{ \
	std::string lib_name_dot = #lib; \
	lib_name_dot.push_back('.'); \
	lua_newtable(L); \
	__VA_ARGS__ \
	lua_setglobal(L, #lib); \
}
