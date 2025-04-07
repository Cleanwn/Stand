#pragma once

#include "CommandPlayerAction.hpp"

#include "PlayerProviderSingle.hpp"
#include "script_handler.hpp"
#include "ScriptHostUtil.hpp"
#include "script_thread.hpp"

namespace Stand
{
	class CommandPlayerGiveSh : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerGiveSh(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("GIVESH"), CMDNAMES("givesh"), NOLABEL, COMMANDPERM_FRIENDLY)
		{
		}

		void onClick(Click& click) final
		{
			if (!click.inOnline())
			{
				return;
			}
			ensureYieldableScriptThread(click, [this]
			{
				if (ScriptHostUtil::canSafelyTakeIt()) // Avoid people using giveseh as a thunderjoin alternative.
				{
					if (auto fm = GtaThread::fromHash(ATSTRINGHASH("freemode")))
					{
						if (auto netcomp = fm->getNetComponent())
						{
							netcomp->giveHost(PP_PTR_SINGLE->getPlayer().getCNetGamePlayer());
						}
					}
				}
			});
		}
	};
}
