#include "CommandHistoryFriendsNote.hpp"

#include "lang.hpp"
#include "natives.hpp"
#include "fmt/format.h"
#include "PlayerHistory.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandHistoryFriendsNote::CommandHistoryFriendsNote(CommandList* const parent)
		: CommandAction(parent, LOC("PLYNOTESET"), CMDNAMES("friendsnote"), NOLABEL)
	{
	}

	std::wstring CommandHistoryFriendsNote::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
		syntax.append(LANG_GET_W("ARGVAL"));
		return syntax;
	}

	void CommandHistoryFriendsNote::onCommand(Click& click, std::wstring& args)
	{
		auto note = StringUtils::utf16_to_utf8(args);
		args.clear();

		for (const auto& p : PlayerHistory::player_history)
		{
			if (p->isFriend())
			{
				p->note = note;
			}
		}

		PlayerHistory::save();
	}
}
