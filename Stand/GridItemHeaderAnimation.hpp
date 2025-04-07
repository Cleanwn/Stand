#pragma once

#include "GridItemHeader.hpp"

#include <memory>

#include "TextureArray.hpp"

namespace Stand
{
	class GridItemHeaderAnimation : public GridItemHeader
	{
	private:
		const std::shared_ptr<TextureArray> header{};

	public:
		explicit GridItemHeaderAnimation(int16_t width, uint8_t priority, GridItem* force_alignment_to);

		void draw() final;
	};
}
