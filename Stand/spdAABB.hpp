#pragma once

#include "vector.hpp"

namespace rage
{
	// spd = spacialdata
	class spdAABB
	{
	public:
		Vec4V m_min; // w-component is user data
		Vec4V m_max; // w-component is user data

		[[nodiscard]] int GetUserInt1() const noexcept
		{
			return m_min.GetWi();
		}

		[[nodiscard]] int GetUserInt2() const noexcept
		{
			return m_max.GetWi();
		}
	};
}
