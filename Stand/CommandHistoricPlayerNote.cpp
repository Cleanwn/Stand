#include "CommandHistoricPlayerNote.hpp"

#include <soup/string.hpp>

#include "CommandHistoricPlayer.hpp"
#include "PlayerHistory.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandHistoricPlayerNote::CommandHistoricPlayerNote(CommandList* parent, const CommandName& simple_name)
		: CommandInput(parent, LOC("PLYNOTE"), std::vector<CommandName>(1, std::move(CommandName(CMDNAME("historynote")).append(simple_name))), NOLABEL, std::string(), ((CommandHistoricPlayer*)parent)->player->note.empty() ? std::string() : std::string(((CommandHistoricPlayer*)parent)->player->note), CMDFLAGS_ACTION_VALUE | CMDFLAG_TEMPORARY)
	{
	}

	void CommandHistoricPlayerNote::onCommand(Click& click, std::wstring& args)
	{
		value = StringUtils::utf16_to_utf8(args);
		soup::string::trim(value);
		args.clear();
		if (value == parent->as<CommandHistoricPlayer>()->player->note)
		{
			click.setResponse(LOC("PLYNOTE_T"));
			return;
		}
		update_state(click);
		parent->as<CommandHistoricPlayer>()->player->note = value;
		PlayerHistory::save();
	}

	void CommandHistoricPlayerNote::onTickInGameViewport()
	{
		value = parent->as<CommandHistoricPlayer>()->player->note;
	}

	void CommandHistoricPlayerNote::onTickInWebViewport()
	{
		return onTickInWebViewportImplRedirect();
	}
}
