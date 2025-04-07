#include "CommandPlayerCrash.hpp"

#include "Hooking.hpp"
#include "RateLimit.hpp"
#include "TransitionHelper.hpp"
#ifndef STAND_DEBUG
#include "yeet_flow.hpp"
#endif

namespace Stand
{
	static RateLimit crash_ratelimit{ 10000, 1 }; // once every 10 seconds
	static RateLimit scripted_crash_ratelimit{ 60000, 1 }; // once every minute

	CommandPlayerCrash::CommandPlayerCrash(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, commandflags_t flags)
		: CommandPlayerAction(parent, std::move(menu_name), std::move(command_names), std::move(help_text), COMMANDPERM_TOXIC, flags)
	{
	}

	void CommandPlayerCrash::onClick(Click& click)
	{
		DEF_P2;
#ifndef STAND_DEBUG
		if (pp->isUser())
		{
			warnSelfHarm(click, &yeet_flow);
			return;
		}
#endif
		if (click.inOnline())
		{
			auto targets = pp->getPlayers();

			if (targets.size() == 1) // Single target?
			{
				if (crash_ratelimit.canRequest())
				{
					crash_ratelimit.addRequest();
				}
				else // User is spamming individual player crashes?
				{
#if false
					// Check if user has Ultimate edition
					if (!click.isUltimateEdition())
#endif
					{
						return;
					}
				}

				if (menu_name.getLocalisationHash() != ATSTRINGHASH("PLYCRSH_E")) // Not Elegant crash?
				{
					// Check if user has Regular edition
					if (!click.isRegularEdition())
					{
						return;
					}
				}
			}
			else // Multiple targets?
			{
#ifdef STAND_DEBUG
				// Check if user has Ultimate edition
				if (!click.isUltimateEdition())
#endif
				{
					return;
				}
			}

			if (click.type == CLICK_SCRIPTED)
			{
				if (!scripted_crash_ratelimit.canRequest())
				{
					return;
				}
				scripted_crash_ratelimit.addRequest();
			}

			bool all_stand_users = !targets.empty();
			for (const auto& p : targets)
			{
				if (!p.isStandUser())
				{
					all_stand_users = false;
				}
			}
			if (all_stand_users)
			{
				click.setResponse(LOC("SUI_ET"));
			}
			ensureYieldableScriptThread(click, [this, targets{ std::move(targets) }]() mutable
			{
				if (GET_MILLIS_SINCE(Click::last_successful_permission_check) <= 500
					|| (targets.size() == 1 && menu_name.getLocalisationHash() == ATSTRINGHASH("PLYCRSH_E"))
					)
				{
					for (const auto& p : targets)
					{
						p.onPreRemove();
					}

					expandDong(std::move(targets));
				}
			});
		}
	}
}
