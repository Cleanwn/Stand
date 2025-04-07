#pragma once

#include <cstdint>

#include "dx_common.hpp"

#include "Alignment.hpp"
#include "BackgroundBlur.hpp"
#include "GridItemType.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class GridItem
	{
	public:
		const GridItemType type;
		int16_t x = SHRT_MIN;
		int16_t y = SHRT_MIN;
		int16_t width;
		int16_t height;
		uint8_t priority;
		Alignment alignment_relative_to_last;
		GridItem* force_alignment_to;
		bool keep_pos = false;

		BackgroundBlur bgblur;

		explicit GridItem(GridItemType type, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last = ALIGN_BOTTOM_LEFT, GridItem* force_alignment_to = nullptr);
		virtual ~GridItem() = default;

		[[nodiscard]] bool occupies(int16_t x, int16_t y) const;

		virtual void onPositioned();

		virtual void draw();
	protected:
		void drawBackground(const DirectX::SimpleMath::Color& colour);
		void drawBackgroundRect(const DirectX::SimpleMath::Color& colour) const;
		void drawBackgroundEffects();
		void drawBackgroundEffects(const DirectX::SimpleMath::Color& bgColour);
		void drawBackgroundBlur();
	};
#pragma pack(pop)
}
