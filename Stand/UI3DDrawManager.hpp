#pragma once

#include "struct_base.hpp"

#include "atFixedArray.hpp"

namespace rage
{
	struct Color32
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};
	static_assert(sizeof(Color32) == 4);
}

// UI3D stuff is "disabled" by the alpha being forced to 0.0f, but you can still do:
// 
// if (GRAPHICS::UI3DSCENE_IS_AVAILABLE())
// {
// 	if (GRAPHICS::UI3DSCENE_PUSH_PRESET("CELEBRATION_WINNER"))
// 	{
// 		// -Y = Push away, Z = Elevation
// 		GRAPHICS::UI3DSCENE_ASSIGN_PED_TO_SLOT("CELEBRATION_WINNER", g_player_ped, 0, 0.0f, 0.0f, 0.0f);
// 	}
// }
//
// and then adjust the freshly-pushed UISceneObjectPatchedData.

#define UI_MAX_SCENE_PRESET_ELEMENTS 5
#define UI_LIGHT_RIG_COUNT 4

struct UISceneObjectData
{
	PAD(0, 0x10) rage::Vector3 position;
	/* 0x20 */ rage::Vector3 position_43;
	/* 0x30 */ rage::Vector3 rotation;
	/* 0x40 */ float screen_x;
	/* 0x44 */ float screen_y;
	/* 0x48 */ float screen_width;
	/* 0x4C */ float screen_height;
	PAD(0x50, 0x60) rage::Color32 bg_colour;
	/* 0x64 */ rage::Color32 unk_colour; // supposed to be blend colour
	PAD(0x68, 0x80);
};
static_assert(sizeof(UISceneObjectData) == 0x80);

struct UIScenePreset
{
	uint32_t name;
	PAD(0x04, 0x10) rage::atFixedArray<UISceneObjectData, UI_MAX_SCENE_PRESET_ELEMENTS> elements;
};
static_assert(sizeof(UIScenePreset) == 0x10 + sizeof(rage::atFixedArray<UISceneObjectData, UI_MAX_SCENE_PRESET_ELEMENTS>));

struct UISceneObjectPatchedData
{
	/* 0x00 */ CPed* ped;
	PAD(0x08, 0x54) float alpha;
	/* 0x58 */ float light_intensity;
	PAD(0x5C, 0x60);
};
static_assert(sizeof(UISceneObjectPatchedData) == 0x60);

struct UILightData
{
	/* 0x00 */ rage::Vector3 pos;
	/* 0x10 */ float intensity;
	/* 0x14 */ float radius;
	/* 0x18 */ float falloff;
	/* 0x1C */ rage::Color32 colour;
};
static_assert(sizeof(UILightData) == 0x20);

struct UILightRig
{
	/* 0x00 */ rage::atFixedArray<UILightData, 4> lights;
	PAD(0x84, 0xB0);
};
static_assert(sizeof(UILightRig) == 0xB0);

struct UI3DDrawManager
{
	PAD(0, 0x530) rage::atFixedArray<UILightRig, UI_LIGHT_RIG_COUNT> light_rigs{};
	PAD(0x530 + sizeof(rage::atFixedArray<UILightRig, UI_LIGHT_RIG_COUNT>), 0x820) UISceneObjectPatchedData patched_data[UI_MAX_SCENE_PRESET_ELEMENTS];
	/* 0xA00 */ UIScenePreset* current_preset;
};
static_assert(sizeof(UI3DDrawManager) == 0xA00 + 8);
