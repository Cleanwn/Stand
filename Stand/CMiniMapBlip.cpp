#include "CMiniMapBlip.hpp"

#include "pointers.hpp"

CMiniMapBlip* CMiniMapBlip::fromHandle(int handle)
{
	if (handle != 0)
	{
		CMiniMapBlip* blip = Stand::pointers::blips[(uint16_t)handle];
		if (blip != nullptr
			&& (handle >> 16) == blip->handle_hi
			)
		{
			return blip;
		}
	}
	return nullptr;
}
