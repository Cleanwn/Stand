#pragma once

#include <cstdint>

namespace rage
{
	struct mthRandom
	{
		uint32_t m_Seed0;
		uint32_t m_Seed1;
		bool m_StoredGaussianExists;
		float m_StoredGaussian;

		mthRandom(int seed)
		{
			Reset(seed);
		}

		void Reset(int seed)
		{
			const unsigned useed = (unsigned)seed;
			m_Seed0 = useed + !useed;   // Make sure zero doesn't happen
			m_Seed1 = ((useed << 16) | (useed >> 16)) ^ useed;
			m_StoredGaussianExists = false;
			m_StoredGaussian = 0.0f;
		}

		[[nodiscard]] int GetInt()
		{
			uint64_t temp = (uint64_t)1557985959 * m_Seed0 + m_Seed1;
			m_Seed0 = uint32_t(temp & ~0u);
			m_Seed1 = uint32_t((temp >> 32) & ~0u);
			// All 32 bits are random but it would break too much code
			// if the sign bit was left in.
			return m_Seed0 & (~0u >> 1);
		}

		[[nodiscard]] int GetRanged(int m, int M)
		{
			return((GetInt() % (M - m + 1)) + m);
		}
	};
}
