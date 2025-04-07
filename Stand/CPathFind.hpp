#pragma once

#include "struct_base.hpp"

#include "datBase.hpp"
#include "pgBase.hpp"

#include <Windows.h>

namespace rage
{
	class strStreamingModule : public datBase
	{
	};
}

class CNodeAddress
{
public:
	union
	{
		uint32_t m_regionAndIndex;
		struct 
		{
			uint16_t region;
			uint16_t index;
		};
	};
};
static_assert(sizeof(CNodeAddress) == 4);

class CPathNode
{
public:
	/* 0x00 */ CPathNode* m_pNext;
	/* 0x08 */ CPathNode* m_pPrevious;
	/* 0x10 */ CNodeAddress	m_address;
	/* 0x14 */ uint32_t m_streetNameHash;
	/* 0x18 */ int16_t m_distanceToTarget;
	/* 0x1A */ int16_t m_startIndexOfLinks;
	/* 0x1C */ int16_t x;
	/* 0x1E */ int16_t y;
	uint16_t m_group : 3;
	uint16_t m_Offroad : 1;
	uint16_t m_onPlayersRoad : 1;
	uint16_t m_noBigVehicles : 1;
	uint16_t m_cannotGoRight : 1;
	uint16_t m_cannotGoLeft : 1;
	uint16_t m_slipLane : 1;
	uint16_t m_indicateKeepLeft : 1;
	uint16_t m_indicateKeepRight : 1;
	uint16_t m_specialFunction : 5;
	/* 0x22 */ int16_t z;
	uint32_t m_noGps : 1;
	uint32_t m_closeToCamera : 1;
	uint32_t m_slipJunction : 1;
	uint32_t m_alreadyFound : 1;
	uint32_t m_switchedOffOriginal : 1;
	uint32_t m_waterNode : 1;
	uint32_t m_highwayOrLowBridge : 1;
	uint32_t m_switchedOff : 1;
	uint32_t m_qualifiesAsJunction : 1;
	uint32_t m_speed : 2;
	uint32_t m_numLinks : 5;
	uint32_t m_inTunnel : 1;
	uint32_t m_distanceHash : 7;
	uint32_t m_deadEndness : 3;
	uint32_t m_leftOnly : 1;
};
static_assert(sizeof(CPathNode) == 0x28);

class CPathNodeLink
{
	CNodeAddress m_OtherNode;
	uint32_t flags;
};
static_assert(sizeof(CPathNodeLink) == 0x08);

class CPathRegion : public rage::pgBase
{
public:
	/* 0x10 */ CPathNode* aNodes;
	/* 0x18 */ int32_t NumNodes;
	/* 0x1C */ int32_t NumNodesCarNodes;
	/* 0x20 */ int32_t NumNodesPedNodes;
	/* 0x28 */ CPathNodeLink* aLinks;
};
static_assert(sizeof(CPathRegion) == 0x28 + 8);

#define	PATHFINDMAPSPLIT 32
#define	PATHFINDREGIONS (PATHFINDMAPSPLIT * PATHFINDMAPSPLIT)

class CPathFind : public rage::strStreamingModule
{
	INIT_PAD(rage::strStreamingModule, 0x1640) CPathRegion* apRegions[PATHFINDREGIONS];

	[[nodiscard]] bool IsRegionLoaded(const CNodeAddress node) const
	{
		return node.region < PATHFINDREGIONS
			&& apRegions[node.region]
			&& apRegions[node.region]->aNodes
			;
	}

	[[nodiscard]] CPathNode* FindNodePointer(CNodeAddress node)
	{
		if (IsRegionLoaded(node))
		{
			return &(apRegions[node.region]->aNodes)[node.index];
		}
		return nullptr;
	}

	[[nodiscard]] CPathNodeLink& GetNodesLink(const CPathNode& node, int32_t i)
	{
		return apRegions[node.m_address.region]->aLinks[node.m_startIndexOfLinks + i];
	}
};
static_assert(sizeof(CPathFind) == 0x1640 + (8 * PATHFINDREGIONS));
