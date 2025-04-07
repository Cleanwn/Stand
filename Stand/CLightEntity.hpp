#pragma once

#include "gta_entity.hpp"

#include "fwRegdRef.hpp"
#include "spdAABB.hpp"

enum e2dEffectType
{
	ET_LIGHT = 0,
};

class CLightEntity : public CEntity
{
public:
	/* 0xD0 */ rage::fwRegdRef<CEntity> m_parentEntity;
	PAD(0xD8, 0xE0) rage::spdAABB m_boundBox;

	[[nodiscard]] e2dEffectType Get2dEffectType() const noexcept
	{
		return (e2dEffectType)(m_boundBox.GetUserInt1() & 0xFF);
	}
};
static_assert(offsetof(CLightEntity, m_parentEntity) == 0xD0);
static_assert(offsetof(CLightEntity, m_boundBox) == 0xE0);
