#pragma once

#include "CommandToggle.hpp"

#include "DisableDripfeed.hpp"

namespace Stand
{
	class CommandDisableDripfeed : public CommandToggle
	{
	public:
		explicit CommandDisableDripfeed(CommandList* const parent)
			: CommandToggle(parent, LOC("NODRIPFEED"), combineCommandNames(CMDNAMES("no", "disable", "bypass"), CMDNAME("dripfeeding")), LOC("NODRIPFEED_H"), true)
		{
		}

		void onEnable(Click& click) final
		{
			DisableDripfeed::enabled = TRUE;
		}

		void onDisable(Click& click) final
		{
			DisableDripfeed::enabled = FALSE;
			DisableDripfeed::onDisable();
		}
	};
}
