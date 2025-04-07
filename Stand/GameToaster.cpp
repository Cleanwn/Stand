#include "GameToaster.hpp"

#include "natives.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"

namespace Stand
{
	int GameToaster::doToast(const std::wstring& message) const
	{
		auto msg{ message };
		UnicodePrivateUse::destroyGta(msg);
		UnicodePrivateUse::toGta(msg);
		StringUtils::replace_all(msg, L"&", L"&amp;");
		StringUtils::replace_all(msg, L"<", L"&lt;");
		StringUtils::replace_all(msg, L">", L"&gt;");
		//StringUtils::replace_all(msg, L"「", L"[");
		//StringUtils::replace_all(msg, L"」", L"]");
		HUD::THEFEED_SET_BACKGROUND_COLOR_FOR_NEXT_POST(Util::get_closest_hud_colour(int(g_renderer.notifyBorderColour.x * 255.0f), int(g_renderer.notifyBorderColour.y * 255.0f), int(g_renderer.notifyBorderColour.z * 255.0f), int(g_renderer.notifyBorderColour.w * 255.0f)));
		Util::BEGIN_TEXT_COMMAND_THEFEED_POST(StringUtils::utf16_to_utf8(msg));
		return HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(false, true);
	}

	void GameToaster::toast(const std::wstring& message) const
	{
		doToast(message);
	}

	void GameToaster::setPersistentToast(const std::wstring& message)
	{
		if (persistent != 0)
		{
			clearPersistentToast();
		}
		HUD::THEFEED_FREEZE_NEXT_POST();
		persistent = doToast(message);
	}

	void GameToaster::clearPersistentToast()
	{
		HUD::THEFEED_REMOVE_ITEM(persistent);
		persistent = 0;
	}
}
