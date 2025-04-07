#pragma once

namespace rage
{
	enum
	{
		INTLOC_INVALID_INDEX = -1,
		INTLOC_ROOMINDEX_LIMBO = 0,
		INTLOC_MAX_INTERIOR_INDEX = 1200,
		INTLOC_MAX_INTERIOR_ROOMS_COUNT = 32,
		INTLOC_MAX_INTERIOR_PORTALS_COUNT = 256,
		INTLOC_MAX_ENTITIES_IN_ROOM_0 = 12
	};

	union fwInteriorLocation
	{
		uint32_t as_uint32;
		struct
		{
			int16_t m_interiorIndex;
			uint16_t m_isAttachedToPortal : 1;
			uint16_t m_isPortalToExterior : 1;
			int16_t m_roomOrPortalIndex : 14;
		} as_interiorLocation;

		fwInteriorLocation()
			: as_uint32(0)
		{
			as_interiorLocation.m_interiorIndex = INTLOC_INVALID_INDEX;
			as_interiorLocation.m_roomOrPortalIndex = INTLOC_INVALID_INDEX;
		}
	};
	static_assert(sizeof(fwInteriorLocation) == 4);
}
