#pragma once

#include "GridItem.hpp"

#include "fwddecl.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class GridItemScrollbar : public GridItem
	{
	private:
		CommandList* view;
		int16_t thumb_offset;
		int16_t thumb_height;

	public:
		explicit GridItemScrollbar(CommandList* view, int16_t width, int16_t height, uint8_t priority);
		void update();

		void draw() final;
	};
#pragma pack(pop)
}
