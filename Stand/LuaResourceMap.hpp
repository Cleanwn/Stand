#pragma once

#include <lauxlib.h>

#include "luaS_push.hpp"

namespace Stand
{
	template <typename T>
	class LuaResourceMap
	{
	public:
		std::unordered_set<T*> instances{};

		int construct(lua_State* L, T* inst)
		{
			instances.emplace(inst);
			luaS_push(L, inst);
			return 1;
		}

		[[nodiscard]] bool empty() const noexcept
		{
			return instances.empty();
		}

		[[nodiscard]] size_t size() const noexcept
		{
			return instances.size();
		}

		[[nodiscard]] bool contains(T* inst) const noexcept
		{
			return instances.contains(inst);
		}

		void free(lua_State* L)
		{
			auto inst = reinterpret_cast<T*>(luaL_checkinteger(L, 1));
			const auto i = instances.find(inst);
			if (i != instances.end())
			{
				instances.erase(i);
				delete inst;
			}
		}

		void freeAll()
		{
			for (auto& inst : instances)
			{
				delete inst;
			}
			instances.clear();
		}
	};
}
