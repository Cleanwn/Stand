#pragma once

#include "GridItem.hpp"

namespace Stand
{
	class GridItemTextBigCentre : public GridItem
	{
	private:
		const std::wstring text;

	public:
		explicit GridItemTextBigCentre(std::wstring&& text, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, GridItem* force_alignment_to);

		void draw() override;
	protected:
		void drawText() const;
	};
}
