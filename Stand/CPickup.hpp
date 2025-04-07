#pragma once

#include "CObject.hpp"

class CPickup : public CObject
{
	INIT_PAD(CObject, 0x440) rage::Vector3 m_StartingLinearVelocity;
	/* 0x450 */ rage::Vector3 m_StartingAngularVelocity;
	/* 0x460 */ CPed* m_lastOwner;
	/* 0x468 */ uint32_t m_pickupHash;
	class CPickupData* m_pPickupData;
};
