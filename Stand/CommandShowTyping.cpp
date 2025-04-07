#include "CommandShowTyping.hpp"

#include <fmt/core.h>

#include "AbstractPlayer.hpp"
#include "Chat.hpp"
#include "lang.hpp"
#include "pointers.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"
#include "tbScreenshotMode.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandShowTyping::CommandShowTyping(CommandList* const parent)
		: CommandToggle(parent, LOC("TPNGIDCTR2"), CMDNAMES("typingindicator"), LOC("TPNGIDCTR_H"))
	{
		instance = this;
	}

	void CommandShowTyping::onEnable(Click&)
	{
		rePresentEvent::registerHandler(&presentCallback);
	}

	void CommandShowTyping::onDisable(Click&)
	{
		rePresentEvent::unregisterHandler(&presentCallback);
	}

	void CommandShowTyping::presentCallback()
	{
		if (*pointers::CLoadingScreens_ms_Context != 0
			|| g_tb_screenshot_mode.isEnabled()
			)
		{
			return;
		}
		std::vector<std::string> typing_players = {};
		for (const auto& p : AbstractPlayer::listExcept(true))
		{
			if (p.isTyping())
			{
				typing_players.emplace_back(p.getName());
			}
		}
		std::string typing_indicator;
		if (typing_players.empty())
		{
			if (preview == NONE)
			{
				return;
			}
			typing_indicator = LANG_GET("IPSUM");
		}
		else if (typing_players.size() == 1)
		{
			typing_indicator = LANG_FMT("TPNGIDCTR_1", fmt::arg("a", typing_players.at(0)));
		}
		else
		{
			typing_indicator = LANG_FMT("TPNGIDCTR_M", StringUtils::concatListUsingLang(std::move(typing_players)));
		}
		Position2d pos;
		if (preview == NONE ? Chat::isOpen() : preview == OPEN)
		{
			pos = pos_open;
		}
		else
		{
			pos = pos_closed;
		}
		g_renderer.drawTextSmallH(pos.x, pos.y - (g_renderer.small_text.scale / float(TEXT_HEIGHT_PX)), StringUtils::utf8_to_utf16(typing_indicator).c_str(), g_renderer.hudColour);
	}
}
