#pragma once

#include "CommandAction.hpp"
#include "CommandWithOnTickFocused.hpp"

#include "DrawUtil3d.hpp"
#include "Gui.hpp"
#include "pointers.hpp"
#include "script_events.hpp"

namespace Stand
{
	class CommandPlayerAptWp : public CommandWithOnTickFocused<CommandAction>
	{
	public:
		explicit CommandPlayerAptWp(CommandList* const parent)
			: CommandWithOnTickFocused(parent, LOC("NEAR_WP"), { CMDNAME("aptwp") }, NOLABEL, 0, COMMANDPERM_RUDE)
		{
			PC_PTR->registerCommand(this);
		}

		void onTickFocused() final
		{
			if (!g_gui.waypoint.isNull())
			{
				DrawUtil3d::draw_line_and_ar_beacon(AptHelper::getNearestPos(g_gui.waypoint));
			}
		}

		void onClick(Click& click) final
		{
			auto wp = click.getEffectiveIssuer().getWaypoint();
			if (!wp.has_value())
			{
				click.setResponse(LOC("BLIPNFND"));
			}
			else
			{
				ensureScriptThread(click, [this, click, wp{ wp.value() }]
				{
					DEF_P2;
					const auto players = pp->getPlayers(pp->single);
					const uint8_t pos_id = AptHelper::getNearestPosId(wp);
					if (!*pointers::is_session_started)
					{
						TpUtil::teleport(AptHelper::getPosFromId(pos_id));
					}
					else
					{
						const uint8_t property_id = AptHelper::posIdToPropertyId(pos_id);
						for (const AbstractPlayer p : players)
						{
							uint64_t args[] = {
								Util::i32_to_u64_trickery(SE_APARTMENT_INVITE),
								g_player, // 1: Sender
								click.issuer_is_explicit ? click.issued_for : Util::get_invalid_player_id(p), // 2: Owner, won't show "invited you into their apartment" if player is not in session
								Util::i32_to_u64_trickery(-1),
								1,
								property_id, // 5: Property ID
								0,
								0,
								0,
								0,
							};
							TSE(1 << p, args);
						}
					}
				});
			}
		}
	};
}
