#pragma once

#include "CommandPlayerAction.hpp"

#include "script_events.hpp"
#include "tse.hpp"

namespace Stand
{
	class CommandPlayerDisableDrivingVehicles : public CommandPlayerAction
	{
	public:
		explicit CommandPlayerDisableDrivingVehicles(CommandList* const parent)
			: CommandPlayerAction(parent, LOC("PLYDDVEH"), { CMDNAME("novehs") }, LOC("PLYDDVEH_H"), COMMANDPERM_RUDE)
		{
		}

		void onClick(Click& click) final
		{
			if (click.inOnline())
			{
				ensureScriptThread(click, [this, click]
				{
					uint64_t args[] = {
						Util::i32_to_u64_trickery(SE_APARTMENT_INVITE),
						g_player, // 1: Sender
						Util::get_invalid_player_id(), // 2: Owner
						Util::i32_to_u64_trickery(-1),
						1,
						AptHelper::max_property_id + 1, // 5: Property ID
						0,
						0,
						0,
						0,
					};
					DEF_P2;
					TSE(Util::get_session_players_bitflag(pp->getPlayers(pp->single)), args);
				});
			}
		}
	};
}
