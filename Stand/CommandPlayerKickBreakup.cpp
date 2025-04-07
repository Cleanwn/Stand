#include "CommandPlayerKickBreakup.hpp"

#include "PlayerProvider.hpp"

namespace Stand
{
	CommandPlayerKickBreakup::CommandPlayerKickBreakup(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, int remove_reason)
		: CommandPlayerActionAggressiveCustom(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), remove_reason(remove_reason)
	{
	}

	void CommandPlayerKickBreakup::onClick(Click& click)
	{
		if (click.inOnline() && click.isRegularEdition())
		{
			DEF_P2;
			if (pp->isUser())
			{
				click.notAvailableOnUser();
				return;
			}
			std::vector<AbstractPlayer> targets = pp->getPlayers();
			if (targets.empty())
			{
				return;
			}
#if !HAVE_BREAKUP_KICK
			if (!g_player.isHost())
			{
				click.setResponse(LOC("CMDNEEDHOST"));
				return;
			}
#endif
			if (g_player.isHost())
			{
				click.setResponse(LOC("PLYKCK_T_H"));
			}
			const int remove_reason = this->remove_reason; // Capturing remove_reason directly instead of `this` to save 4 bytes
			ensureScriptThread(click, [targets{std::move(targets)}, remove_reason]
			{
				for (const auto& target : targets)
				{
					if (target.canUseUnblockableKick())
					{
						target.onPreRemove();
					}
					target.kickBreakup(remove_reason);
				}
			});
		}
	}
}
