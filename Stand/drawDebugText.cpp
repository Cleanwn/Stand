#include "drawDebugText.hpp"

#include "lang.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "tbScreenshotMode.hpp"

#define DDT_THREAD_SAFE false
#define DDT_SPAM_SAFE false

#if DDT_THREAD_SAFE
#else
#include "ExecCtx.hpp"
#endif

#if DDT_SPAM_SAFE
#include "huddecl.hpp"
#endif

namespace Stand
{
	inline bool g_force_debug_text = false;

	static std::wstring debug_text;

#if DDT_THREAD_SAFE
	static Spinlock mtx{};
#endif

	void onTick_drawDebugText()
	{
#if DDT_THREAD_SAFE
		std::lock_guard lock(mtx);
#endif
		if (!debug_text.empty())
		{
#if DDT_SPAM_SAFE
			debug_text = g_renderer.wrapTextH(std::move(debug_text), g_renderer.info_scale, HUD_WIDTH);
#endif
			if (!g_tb_screenshot_mode.isEnabled())
			{
				g_renderer.drawTextThisTickH(g_renderer.info_x, g_renderer.info_y, std::move(debug_text), g_renderer.info_alignment, g_renderer.info_scale, g_renderer.hudColour, g_renderer.m_font_user, false, {}, g_renderer.info_drop_shadow);
			}
			debug_text.clear();
		}
		else if (g_force_debug_text)
		{
			if (!g_tb_screenshot_mode.isEnabled())
			{
				g_renderer.drawTextThisTickH(g_renderer.info_x, g_renderer.info_y, LANG_GET_W("IPSUM"), g_renderer.info_alignment, g_renderer.info_scale, g_renderer.hudColour, g_renderer.m_font_user, false, {}, g_renderer.info_drop_shadow);
			}
		}
	}

	void drawDebugText(const std::wstring& str)
	{
#if DDT_THREAD_SAFE
		std::lock_guard lock(mtx);
#else
		SOUP_ASSERT(ExecCtx::get().isScript());
#endif
		if (debug_text.empty())
		{
			debug_text = str;
		}
		else
		{
#if DDT_SPAM_SAFE
			debug_text.append(L", ");
#else
			debug_text.push_back(L'\n');
#endif
			debug_text.append(str);
		}
	}

	void drawDebugText(std::wstring&& str)
	{
#if DDT_THREAD_SAFE
		std::lock_guard lock(mtx);
#else
		SOUP_ASSERT(ExecCtx::get().isScript());
#endif
		if (debug_text.empty())
		{
			debug_text = std::move(str);
		}
		else
		{
#if DDT_SPAM_SAFE
			debug_text.append(L", ");
#else
			debug_text.push_back(L'\n');
#endif
			debug_text.append(std::move(str));
		}
	}

	void drawDebugText(const StringCastable& str)
	{
		drawDebugText(StringUtils::utf8_to_utf16(str));
	}

	void drawInfoText(std::string&& value, const std::string& name)
	{
		switch (g_renderer.info_alignment & 0b11)
		{
		default:
			drawDebugText(std::move(std::string(name).append(": ").append(std::move(value))));
			break;

		case 0b10: // Right
			drawDebugText(std::move(value.append(" - ").append(name)));
			break;
		}
	}
}
