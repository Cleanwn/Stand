#include "CommandRestartfm.hpp"

#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "Script.hpp"
#include "script_handler.hpp"
#include "ScriptHostUtil.hpp"
#include "script_thread.hpp"
#include "ScriptVmErrorHandling.hpp"

namespace Stand
{
	CommandRestartfm::CommandRestartfm(CommandList* const parent)
		: CommandAction(parent, LOC("RSTFM"), CMDNAMES("restartfm"))
	{
	}

	void CommandRestartfm::onClick(Click& click)
	{
		if (!click.inOnline())
		{
			return;
		}
		click.stopInputIfAllowed();
		ensureYieldableScriptThread(click, []
		{
			if (auto t = GtaThread::fromHash(ATSTRINGHASH("freemode")))
			{
				// Avoid possibly getting stuck in PRE_FM_LAUNCH_SCRIPT later
				if (auto netcomp = t->getNetComponent();
					netcomp && netcomp->amHost()
					)
				{
					netcomp->giveHost(ScriptHostUtil::getViableSh().getCNetGamePlayer());
				}

				// Kill fm
				t->kill();

				// Let it process that fm just died
				Script::current()->yield();

				// If someone starts typing now, we wouldn't know, so give silent chat chance to everyone
				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					SOUP_IF_LIKELY (auto cmd = p.getCommand())
					{
						cmd->has_silentchat_chance = true;
					}
				}
			}

			ScriptVmErrorHandling::restartFreemode();
		});
	}
}
