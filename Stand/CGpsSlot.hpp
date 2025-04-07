#pragma once

#include "gta_fwddecl.hpp"
#include "vector.hpp"

enum eGpsNodePosInfo
{
	GNI_IGNORE_FOR_NAV = 0x00008000,
	GNI_PLAYER_TRAIL_POS = 0x00004000,
};

class CGpsSlot
{
public:
	/* 0x00 */ rage::Vector3* m_NodeCoordinates;
	/* 0x08 */ CNodeAddress* m_NodeAdress;
	/* 0x10 */ uint16_t* m_NodeInfo;
	/* 0x18 */ int16_t* m_NodeDistanceToTarget;
	/* 0x20 */ rage::Vector3 m_Destination;
	/* 0x30 */ rage::Vector3 m_PartialDestination;
	/* 0x40 */ int32_t m_iScriptIdSetGpsFlags;
	/* 0x44 */ int32_t m_NumNodes;
	uint32_t m_LastRecalcRouteTime;
	uint32_t m_LastPartialRouteTime;
	uint32_t m_LastShiftRouteTime;
	uint32_t m_iScanNodesTimer;
	uint32_t m_RedrawTimer;
	int32_t m_Identifier;
	int32_t m_SlotIndex;
	float m_fShowRouteProximity;
	float m_LastRouteDistance;
	int16_t m_iGpsFlags;
	bool m_bClearRender;
	uint8_t m_uCustomRouteFlags;
	uint8_t m_fastCalculatingPhase : 1;
	uint8_t m_targetEntitySpecified : 1;
	uint8_t m_bUsePartialDestination : 1;
	uint8_t m_bLastRouteSearchWasIgnoreOneWay : 1;
	bool m_bCurrentRenderStatus : 1;
	bool m_bRenderedLastFrame : 1;
	bool m_AlreadyAnnouncedCalculatingRoute : 1;
	bool m_AlreadyAnnouncedHighlighted : 1;
	bool m_InCarLastUpdate : 1;
	bool m_LastAnnouncedWasADouble : 1;
	/* 0x74 */ int32_t m_Status;
	PAD(0x78, 0xB0);

	[[nodiscard]] bool isPartialRoute() const noexcept
	{
		if (m_NumNodes != 0)
		{
			auto end_pos = m_NodeCoordinates[m_NumNodes - 1];
			return end_pos.distance(m_PartialDestination) < end_pos.distance(m_Destination);
		}
		return false;
	}
};
static_assert(sizeof(CGpsSlot) == 0xB0);
