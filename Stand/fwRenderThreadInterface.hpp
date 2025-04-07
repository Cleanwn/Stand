#pragma once

#include "sysThreadType.hpp"

namespace rage
{
	class fwRenderThreadInterface
	{
	public:
		PAD(0, 0x58) int32_t m_updateBuffer;
		int32_t m_renderBuffer;

		[[nodiscard]] int32_t GetCurrentBuffer() const
		{
			if (!sysThreadType::IsRenderThread())
			{
				return m_updateBuffer;
			}
			return m_renderBuffer;
		}
	};
}
