#include "CommandSetCarClubRepLevel.hpp"

#include <stdexcept>

#include "get_next_arg.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "ScriptGlobal.hpp"
#include "StatUtil.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandSetCarClubRepLevel::CommandSetCarClubRepLevel(CommandList* parent)
		: CommandAction(parent, LOC("SETREPLVL"), { CMDNAME("setlscarmeetreplevel") }, LOC("SETREPLVL_H"))
	{
	}

	std::wstring CommandSetCarClubRepLevel::getCommandSyntax() const
	{
		return std::move(LANG_GET_W("CMD").append(L": setlscarmeetreplevel [1 ").append(LANG_GET_W("ARGRNGE")).append(L" 1000]"));
	}

	void CommandSetCarClubRepLevel::onClick(Click& click)
	{
		if (click.inOnline())
		{
			CommandAction::onClick(click);
		}
	}

	void CommandSetCarClubRepLevel::onCommand(Click& click, std::wstring& args)
	{
		auto arg = get_next_arg(args);
		if (arg.empty())
		{
			return onClick(click);
		}
		const auto char_slot = Util::getCharSlot(click);
		if (char_slot != -1)
		{
			try
			{
				auto lvl = std::stoi(arg);
				if (lvl > 1000)
				{
					lvl = 1000;
				}
				auto rep = Util::get_car_club_rep_required_for_level(lvl);
				STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CAR_CLUB_REP"), 0, true);
				*ScriptGlobal(GLOBAL_TUNABLE_BASE + GLOBAL_TUNABLE_CARMEET_REP_8_AMOUNT).as<int*>() = rep;
				*ScriptGlobal(GLOBAL_TUNABLE_BASE + GLOBAL_TUNABLE_CARMEET_REP_8_MULTIPLIER).as<float*>() = 1.0f;
				StatUtil::setPackedBool(32257, FALSE, char_slot);
				click.setResponse(LOC("SETREPLVL_T"));
			}
			catch (const std::exception&)
			{
				click.setResponse(LOC("INVARG"));
				click.showCommandBoxIfPossible(L"setlscarmeetreplevel " + arg);
			}
		}
	}
}
