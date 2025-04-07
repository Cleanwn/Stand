#pragma once

#include "CommandPlayerAction.hpp"

#include "Util.hpp"

namespace Stand
{
	class CommandPlayerCopyWaypoint : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerCopyWaypoint(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("COPYWP"), CMDNAMES_OBF("copywp", "copywaypoint"))
		{
		}

		void onClick(Click& click) final
		{
			const auto plr = PP_PTR->getPlayer();

			if (auto wp = plr.getWaypoint(); wp.has_value())
			{
				Util::setWaypoint(wp.value());
			}
			else
			{
				click.setResponse(LIT(LANG_FMT("PLYWPTP_N", plr.getName())));
			}
		}
	};
}