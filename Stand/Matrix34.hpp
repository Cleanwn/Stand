#pragma once

#include <soup/Matrix.hpp>
#include "vector.hpp"

namespace rage
{
	class Matrix34
	{
	public:
		rage::Vector3 a;
		rage::Vector3 b;
		rage::Vector3 c;
		rage::Vector3 d;

		Matrix34(const soup::Matrix& m)
			: a(m.mf[0], m.mf[1], m.mf[2], m.mf[3]),
			  b(m.mf[4], m.mf[5], m.mf[6], m.mf[7]),
			  c(m.mf[8], m.mf[9], m.mf[10], m.mf[11]),
			  d(m.mf[12], m.mf[13], m.mf[14], m.mf[15])
		{
		}
	};
	static_assert(sizeof(Matrix34) == 0x40);
}
