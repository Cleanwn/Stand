#pragma once

#include "struct_base.hpp"

namespace rage
{
	namespace Vec
	{
		typedef __m128 Vector_4V;
	};

	class Mat34V
	{
	public:
		Vec::Vector_4V m_col0;
		Vec::Vector_4V m_col1;
		Vec::Vector_4V m_col2;
		Vec::Vector_4V m_col3;
	};
	static_assert(sizeof(Mat34V) == 0x40);
}
