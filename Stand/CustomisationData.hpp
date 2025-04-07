#pragma once

#include "Mixed.hpp"

namespace Stand
{
	template <typename T>
	class CustomisationData : public T
	{
	private:
		std::array<Mixed, T::_NUM_PROPERIES> data{};

	public:
		[[nodiscard]] Mixed read(int idx) final
		{
			return data.at(idx);
		}

		void write(int idx, Mixed&& value) final
		{
			data.at(idx) = std::move(value);
		}
	};
}
