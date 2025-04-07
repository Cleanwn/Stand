#pragma once

#include "CommandAction.hpp"

#include "get_next_arg.hpp"
#include "Gui.hpp"
#include "Util.hpp"

namespace Stand
{
	class CommandSetRank : public CommandAction
	{
	public:
		explicit CommandSetRank(CommandList* const parent)
			: CommandAction(parent, LOC("SETLVL"), CMDNAMES("setrank", "setlevel"))
		{
		}

		std::wstring getCommandSyntax() const final
		{
			return std::move(LANG_GET_W("CMD").append(L": setrank [1 ").append(LANG_GET_W("ARGRNGE")).append(L" 8000]"));
		}

		void onClick(Click& click) final
		{
			if(click.inOnline())
			{
				CommandAction::onClick(click);
			}
		}

		void onCommand(Click& click, std::wstring& args) final
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
					int rp = Util::get_rp_required_for_rank(std::stoi(arg));
					if (rp == 0)
					{
						rp = 1;
					}
					STATS::STAT_SET_INT(CHAR_STAT(char_slot, "CHAR_SET_RP_GIFT_ADMIN"), rp, true);
					click.setResponse(LOC("GEN_T_NEWSESS"));
				}
				catch (const std::exception&)
				{
					click.setResponse(LOC("INVARG"));
					click.showCommandBoxIfPossible(L"setrank " + arg);
				}
			}
		}
	};
}
