#pragma once

#define ERASE_PE_HEADER !STAND_DEBUG || false

#if ERASE_PE_HEADER
#include <cstdint>

#include <soup/Bytepatch.hpp>
#endif

namespace Stand
{
	class PeHeader
	{
#if ERASE_PE_HEADER
	public:
		inline static soup::Bytepatch pe_header{};

	private:
		inline static uint8_t ref_count = 0;
#endif

	public:
		static void unveil()
		{
#if ERASE_PE_HEADER
			if (++ref_count == 1)
			{
				pe_header.restore();
			}
#endif
		}

		static void veil()
		{
#if ERASE_PE_HEADER
			if (--ref_count == 0)
			{
				pe_header.initPatchZero();
			}
#endif
		}

		static void dispose()
		{
#if ERASE_PE_HEADER
			pe_header.dispose();
#endif
		}
	};
}
