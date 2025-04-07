#pragma once

#include <forward_list>
#include <optional>

#include "NativeCallContext.hpp"

namespace Stand
{
	struct LuaScriptData
	{
		NativeCallContext native_call_ctx = NativeCallContext();
		std::forward_list<rage::scrVector3> vector3_cache = {};
		bool accept_bools_as_ints = false;
		bool c_style_int_to_bool_casts = true;
		bool accept_nils_as_strings = false;

		std::optional<bool> nilptr{};
		[[nodiscard]] bool isNilptrOptedIn() const noexcept
		{
			return nilptr.has_value() && nilptr.value() == true;
		}
	};
}
