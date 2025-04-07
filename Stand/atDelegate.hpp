#pragma once

namespace rage
{
	// This is actually a templated & macro'd mess
	struct atDelegate
	{
		void* m_Target;
		union
		{
			void* m_MemFunc;
			void* m_FreeFunc;
		} m_Func;
	};
	static_assert(sizeof(atDelegate) == 0x10);
}
