#pragma once

#include "GridItem.hpp"

namespace Stand
{
	class GridItemCommandboxInput : public GridItem
	{
	private:
		const std::wstring text;

	public:
		explicit GridItemCommandboxInput(std::wstring&& text, int16_t width, uint8_t priority);

		void draw() final;
	};
}
