#pragma once

#include "CommandSlider.hpp"

namespace Stand
{
	template <class CSlider>
	class CommandSpoofSlider : public CSlider
	{
	public:
		using CSlider::CSlider;

		void onChange(Click& click, int prev_value) final;
	};
}
