#pragma once

#include "GridItem.hpp"

#include <soup/Canvas.hpp>

namespace Stand
{
	class GridItemQrcode : public GridItem
	{
	private:
		soup::Canvas c;
		//TextureDynamic tex;

	public:
		explicit GridItemQrcode(int16_t size, uint8_t priority, const std::string& text);

		void draw() final;
	};
}
