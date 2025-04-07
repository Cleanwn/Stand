#pragma once

#include <cstdint>

namespace rage
{
	class fwModelId
	{
	public:
		union
		{
			struct {
				uint32_t		m_modelIndex : 16;
				uint32_t		m_mapTypeDefIndex : 12;
				uint32_t		m_bIsStreamed : 1;
				uint32_t		m_bIsMemLocked : 1;
				uint32_t		m_bPad1 : 1;
				uint32_t		m_bPad2 : 1;
			} s;
			uint32_t		m_asU32;
		};
	};
}
