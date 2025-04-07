#pragma once

namespace Stand
{
	struct DebugString
	{
#ifdef STAND_DEBUG
		std::string data;
#endif

		DebugString() = default;

		constexpr DebugString(const char* str)
#ifdef STAND_DEBUG
			: data(str)
#endif
		{
		}

		constexpr DebugString(std::string str)
#ifdef STAND_DEBUG
			: data(std::move(str))
#endif
		{
		}
	};
}
