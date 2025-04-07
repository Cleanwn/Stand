#pragma once

#include "grcTexture.hpp"

// type

namespace rage
{
	struct fwCustomShaderEffectBaseType
	{
	};
}

struct CCustomShaderEffectBaseType : public rage::fwCustomShaderEffectBaseType
{
};

struct CCustomShaderEffectVehicleType : public CCustomShaderEffectBaseType
{
};

struct CCustomShaderEffectWeaponType : public CCustomShaderEffectBaseType
{
};

// effect

namespace rage
{
	struct fwCustomShaderEffect
	{
	};
}

struct CCustomShaderEffectBase : public rage::fwCustomShaderEffect
{
};

class CCustomShaderEffectPed : public CCustomShaderEffectBase
{
	INIT_PAD(CCustomShaderEffectBase, 0x58) rage::grcTexture* head;
	/* 0x60 */ rage::grcTexture* berd;
	/* 0x68 */ rage::grcTexture* hair;
};
static_assert(sizeof(CCustomShaderEffectPed) == 0x70);

struct CCustomShaderEffectVehicle : public CCustomShaderEffectBase
{
	INIT_PAD(CCustomShaderEffectBase, 0xD8) CCustomShaderEffectVehicleType* type;
};
static_assert(sizeof(CCustomShaderEffectVehicle) == 0xD8 + 8);

struct CCustomShaderEffectTree : public CCustomShaderEffectBase
{
};
