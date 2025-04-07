#pragma once

#include "CommandToggle.hpp"

#ifdef STAND_DEBUG
#include "Blacklist.hpp"
#include "Exceptional.hpp"
#endif

namespace Stand
{
	class CommandBlockJoinsAdvertisers : public CommandToggle
	{
	public:
		explicit CommandBlockJoinsAdvertisers(CommandList* const parent)
			: CommandToggle(parent, LOC("BJ_ADS"), {}, NOLABEL, true)
		{
		}

		Label getActivationName() const final
		{
			return LOC("BJ_ADS_A");
		}

		void onChange(Click& click) final
		{
			g_hooking.block_joins_from_known_advertisers = m_on;
		}

#ifdef STAND_DEBUG
		void onTickInGameViewport() final
		{
			EXCEPTIONAL_LOCK(Blacklist::mtx)
			if (!Blacklist::advertisers.empty())
			{
				setHelpText(LIT(fmt::format("[Debug Build] There are currently {} known advertisers.", Blacklist::advertisers.size())));
			}
			EXCEPTIONAL_UNLOCK(Blacklist::mtx)
		}

		void onTickInWebViewport() final
		{
			return onTickInWebViewportImplRedirect();
		}
#endif
	};
}
