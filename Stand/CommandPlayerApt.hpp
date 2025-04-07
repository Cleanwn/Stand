#pragma once

#include "CommandAction.hpp"
#include "CommandWithOnTickFocused.hpp"

#include <fmt/format.h>

#include "DrawUtil3d.hpp"
#include "pointers.hpp"
#include "script_events.hpp"

namespace Stand
{
	class CommandPlayerApt : public CommandWithOnTickFocused<CommandAction>
	{
	private:
		const uint8_t property_id;

	public:
		explicit CommandPlayerApt(CommandList* const parent, const uint8_t property_id)
			: CommandWithOnTickFocused(parent, LIT(AptHelper::propertyIdToName(property_id)), CMDNAMES("apt"), NOLABEL, 0, COMMANDPERM_RUDE), property_id(property_id)
		{
			if (property_id != 1)
			{
				flags |= CMDFLAG_FEATURELIST_FINISHLIST;
			}
			command_names.at(0).append(utf8ToCmdName(fmt::to_string(property_id)));
			PC_PTR->registerCommand(this);
		}

		void onTickFocused() final
		{
			DrawUtil3d::draw_line_and_ar_beacon(AptHelper::getPosFromId(AptHelper::propertyIdToPosId(property_id)));
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [this, click]
			{
				DEF_P2;
				const auto players = pp->getPlayers(pp->single);
				if (!*pointers::is_session_started)
				{
					TpUtil::teleport(AptHelper::getPosFromId(AptHelper::propertyIdToPosId(property_id)));
				}
				else
				{
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
	};
}
