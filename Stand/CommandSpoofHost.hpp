#pragma once

#include "CommandToggleUltimate.hpp"

#include "SessionSpoofing.hpp"
#include "Tunables.hpp"

namespace Stand
{
	class CommandSpoofHost : public CommandToggleUltimate
	{
	public:
		inline static CommandSpoofHost* instance;

		explicit CommandSpoofHost(CommandList* parent)
			: CommandToggleUltimate(parent, LOC("SPFHST"), { CMDNAME("spoofhost") })
		{
			instance = this;
		}

		void onEnableInner(Click& click) final
		{
			if (g_tunables.getBool(TUNAHASH("DISABLE_SESSION_SPOOFING")))
			{
				click.setResponse(LOC("CMDDISA"));
				setOnIndication(false);
			}
		}

		void onDisableInner(Click& click) final
		{
			SessionSpoofing::onDisable();
		}
	};
}
