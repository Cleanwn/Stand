#pragma once

#include <string>

#include "gta_fwddecl.hpp"
#include "Renderer.hpp"

namespace Stand
{
	struct AbstractRenderer
	{
		virtual void drawLine(const rage::Vector2& pos1, const rage::Vector2& pos2, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const = 0;
		virtual void drawLine(const v3& pos1, const v3& pos2, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const = 0;
		virtual void drawText(const v3& pos, std::wstring&& text, float scale, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const = 0;
	};

	struct AbstractRendererDx : public AbstractRenderer
	{
		void drawLine(const rage::Vector2& pos1, const rage::Vector2& pos2, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const final;
		void drawLine(const v3& pos1, const v3& pos2, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const final;
		void drawText(const v3& pos, std::wstring&& text, float scale, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const final;
	};

	struct AbstractRendererNative : public AbstractRenderer
	{
		void drawLine(const rage::Vector2& pos1, const rage::Vector2& pos2, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const final;
		void drawLine(const v3& pos1, const v3& pos2, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const final;
		void drawText(const v3& pos, std::wstring&& text, float scale, DirectX::SimpleMath::Color& colour = g_renderer.arColour) const final;
	};

	inline AbstractRendererDx g_dx_render{};
	inline AbstractRendererNative g_native_render{};
}
