#pragma once

#include "GridItemPrimaryText.hpp"

#include "Renderer.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class GridItemAddressbar : public GridItemPrimaryText
	{
	private:
		std::wstring cursor_text{};
		int16_t cursor_text_x;

	public:
		explicit GridItemAddressbar(int16_t width);
		
		void onPositioned() final;

		void draw() final;
	};
#pragma pack(pop)
}
