#include "GridItemHeader.hpp"

#include "MenuGrid.hpp"

namespace Stand
{
	GridItemHeader::GridItemHeader(int16_t width, int16_t height, uint8_t priority, GridItem* force_alignment_to)
		: GridItem(GRIDITEM_INDIFFERENT, width, height, priority, ALIGN_TOP_CENTRE, force_alignment_to)
	{
	}
}
