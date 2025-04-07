#pragma once

#include <array>

#include "Mixed.hpp"

namespace Stand
{
	class CustomisationIo
	{
	public:
		[[nodiscard]] virtual int getNumProperties() const noexcept = 0;

		[[nodiscard]] virtual Mixed read(int idx) = 0;
		virtual void write(int idx, Mixed&& value) = 0;

		void copyTo(CustomisationIo& b)
		{
			const int num = getNumProperties();
			for (int i = 0; i < num; ++i)
			{
				b.write(i, read(i));
			}
		}
	};
}
