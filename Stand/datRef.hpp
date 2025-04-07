#pragma once

namespace rage
{
	template <typename T>
	struct datRef
	{
		T* ptr;

		operator T* () const noexcept
		{
			return ptr;
		}

		T* operator ->() const noexcept
		{
			return ptr;
		}
	};
}
