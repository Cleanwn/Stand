#pragma once

#ifdef STAND_DEBUG

#include "CommandAction.hpp"

#include "AbstractPlayer.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandTestAdFilter : public CommandAction
	{
	public:
		explicit CommandTestAdFilter(CommandList* const parent)
			: CommandAction(parent, LIT("Test Ad String"), { CMDNAME("testad") })
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			auto ad_level = g_player.getMessageAdLevel(args);
			AbstractPlayer::normaliseForAdFilters(args);
			click.setResponse(LIT(fmt::format("Ad level {}, cta {}, pog {}, emp {}", ad_level, g_player.messageHasCallToAction(args), g_player.messageHasPromiseOfGoodThings(args), g_player.messageIsEmptyOrMiscPromotion(args))));
			args.clear();
		}
	};
}

#endif