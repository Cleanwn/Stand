#pragma once

#include "gta_custom_shader_effect.hpp"
#include "gta_draw_handler.hpp"

#include "PedVariationDS.hpp"

class CPedStreamRenderGfx
{
	PAD(0, 0x2C8) CCustomShaderEffectPed* custom_shader_effect;
};

class CPedDrawHandler : public CEntityDrawHandler
{
	INIT_PAD(CEntityDrawHandler, 0x30) CPedStreamRenderGfx* stream_render_gfx;
	/* 0x38 */ class CPedPropRenderGfx* m_pPedPropRenderGfx;
	/* 0x40 */ CPedVariationData m_varData;
	/* 0x140 */ //CPedPropData m_propData;
	//uint32_t m_perComponentWetnessFlags;
};
static_assert(sizeof(CPedDrawHandler) == 0x140);
