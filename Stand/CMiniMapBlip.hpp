#pragma once

#include "struct_base.hpp"
#include "vector.hpp"

#pragma pack(push, 1)
class CMiniMapBlip
{
public:
	/* 0x00 */ int32_t index;
	/* 0x04 */ int32_t handle;
	/* 0x08 */ uint16_t handle_hi;
	/* 0x0A */ bool m_active;
	PAD(0x0B, 0x0C) int32_t m_entity_id;
	/* 0x10 */ rage::Vector3 pos;
	/* 0x20 */ uint32_t m_display_bits;
	uint32_t m_render_bits; //0x0024
	char *m_message; //0x0028
	char pad_0030[8]; //0x0030
	uint32_t m_description_hash; //0x0038
	char pad_003C[4]; //0x003C
	int32_t m_icon; //0x0040
	int16_t m_flash_interval; //0x0044
	int16_t m_flash_timer; //0x0046
	uint32_t m_color; //0x0048
	uint32_t m_secondary_color; //0x004C
	float m_scale_x; //0x0050
	float m_scale_y; //0x0054
	float m_rotation; //0x0058
	uint8_t m_mission_bits; //0x005C
	uint8_t m_priority; //0x005D
	uint8_t m_display_id; //0x005E
	uint8_t m_alpha; //0x005F
	int8_t m_category; //0x0060
	int8_t m_show_number; //0x0061
	char pad_0062[14]; //0x0062

	[[nodiscard]] static CMiniMapBlip* fromHandle(int handle);
};
static_assert(sizeof(CMiniMapBlip) == 0x70);
#pragma pack(pop)
