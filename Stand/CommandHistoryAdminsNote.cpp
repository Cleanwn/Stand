#include "CommandHistoryAdminsNote.hpp"

#include "lang.hpp"
#include "PlayerHistory.hpp"
#include "rockstar_admins.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandHistoryAdminsNote::CommandHistoryAdminsNote(CommandList* const parent)
		: CommandAction(parent, LOC("PLYNOTESET"), CMDNAMES("adminsnote"), LOC("PLYNOTESET_H_A"))
	{
	}

	std::wstring CommandHistoryAdminsNote::getCommandSyntax() const
	{
		std::wstring syntax{ LANG_GET_W("CMD") };
		syntax.append(L": ").append(cmdNameToUtf16(command_names.at(0))).push_back(L' ');
		syntax.append(LANG_GET_W("ARGVAL"));
		return syntax;
	}

	void CommandHistoryAdminsNote::onCommand(Click& click, std::wstring& args)
	{
		auto note = StringUtils::utf16_to_utf8(args);
		args.clear();
		for (const auto& p : PlayerHistory::player_history)
		{
			if (is_admin_rid(p->rid))
			{
				p->note = note;
			}
		}
		PlayerHistory::save();
	}
}
