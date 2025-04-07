#pragma once

#include "struct_base.hpp"

#include <soup/Matrix.hpp>

namespace rage
{
	using Mat44V = soup::Matrix;

	struct grcWindow
	{
		PAD(0, 0x08) float m_NormWidth;
		/* 0x0C */ float m_NormHeight;
	};

	struct grcViewport
	{
		/* 0x000 */ Mat44V m_World44;
		/* 0x040 */ Mat44V m_ModelView;
		/* 0x080 */ Mat44V m_Composite;
		/* 0x0C0 */ Mat44V m_Camera44;
		/* 0x100 */ Mat44V m_View;
		/* 0x140 */ Mat44V m_Projection;
		/* 0x180 */ Mat44V m_FullComposite;
		PAD(0x1C0, 0x410) grcWindow m_Window;
		PAD(0x410 + sizeof(grcWindow), 0x440) int m_LastWidth;
		/* 0x440 */ int m_LastHeight;
		PAD(0x448, 0x490);

		[[nodiscard]] int GetHeight() const
		{
			const float roundingBias = 0.5f;
			return int(roundingBias + m_Window.m_NormHeight * (float)m_LastHeight);
		}

		[[nodiscard]] int GetWidth() const
		{
			const float roundingBias = 0.5f;
			return int(roundingBias + m_Window.m_NormWidth * (float)m_LastWidth);
		}
	};
	static_assert(sizeof(grcViewport) == 0x490); // see imul in CScriptHud::GetScreenPosFromWorldCoord
}
