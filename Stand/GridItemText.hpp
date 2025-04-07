#pragma once

#include "GridItem.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class GridItemText : public GridItem
	{
	private:
		const std::wstring text;
		const int16_t extra_padding;

	public:
		explicit GridItemText(std::wstring&& text, int16_t width, int16_t extra_padding, uint8_t priority, Alignment alignment_relative_to_last = ALIGN_BOTTOM_LEFT, GridItem* force_alignment_to = nullptr);

		void draw() override;
	protected:
		void drawText() const;
	};
#pragma pack(pop)
}
