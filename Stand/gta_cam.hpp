#pragma once

#include <soup/Matrix.hpp>

#include "struct_base.hpp"
#include "fwRefAwareBase.hpp"

#pragma pack(push, 1)
class camFrame : public rage::fwRefAwareBase
{
public:
	/* 0x10 */ soup::Matrix view;
	PAD(0x50, 0x70) float fov;
};
static_assert(sizeof(soup::Matrix) == sizeof(float) * 16);
static_assert(sizeof(camFrame) == 0x70 + 4);

class camBaseObject : public rage::fwRefAwareBase
{
	INIT_PAD(rage::fwRefAwareBase, 0x20) camFrame frame;
};
static_assert(sizeof(camBaseObject) == 0x20 + sizeof(camFrame));

class camBaseCamera : public camBaseObject
{
	INIT_PAD(camBaseObject, 0xF0) uint16_t update_flags; // 0x40 = coord, 0x80 = rot
	PAD(0xF0 + 2, 0x930);
};
static_assert(sizeof(camBaseCamera) == 0x930);

class camScriptedCamera : public camBaseCamera
{
};

class camBaseDirector : public camBaseObject
{
};

class camGameplayDirector : public camBaseDirector
{
	INIT_PAD(camBaseDirector, 0x100) camFrame frame_2;
	PAD(0x100 + sizeof(camFrame), 0x1E0) camFrame frame_3;
};

class camScriptDirector : public camBaseDirector
{
};
#pragma pack(pop)
