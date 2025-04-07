#pragma once

#include "fwddecl.hpp"
#include "GridItem.hpp"

namespace Stand
{
	class GridItemPrimaryText : public GridItem
	{
	protected:
		std::wstring text;

	public:
		explicit GridItemPrimaryText(int16_t width, int16_t height, std::wstring&& text = {});

		void draw() override;
	protected:
		void draw(const TextSettings& settings);
	};
}
