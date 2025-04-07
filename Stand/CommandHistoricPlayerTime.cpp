#include "CommandHistoricPlayerTime.hpp"

#include "CommandHistoricPlayer.hpp"
#include "format_time_since_1970_for_user_locale.hpp"
#include "lang.hpp"

namespace Stand
{
	CommandHistoricPlayerTime::CommandHistoricPlayerTime(CommandList* const parent, Label&& menu_name, Offset<HistoricPlayer, time_t> value_offset)
		: CommandReadonlyValueCopy(parent, std::move(menu_name)), value_offset(value_offset)
	{
	}

	void CommandHistoricPlayerTime::onTickInGameViewport()
	{
		auto time = value_offset.resolve(*parent->as<CommandHistoricPlayer>()->player);
		if (time == 0)
		{
			setValue(LANG_GET_W("NA"));
		}
		else
		{
			setValue(format_time_since_1970_for_user_locale(time));
		}
	}

	void CommandHistoricPlayerTime::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
