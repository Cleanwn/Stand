#pragma once

#include "CommandAction.hpp"
#include "CommandWithOnTickFocused.hpp"

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"
#include "is_session.hpp"
#include "script_events.hpp"
#include "TpUtil.hpp"
#include "tse.hpp"

namespace Stand
{
	class CommandPlayerAptMe : public CommandWithOnTickFocused<CommandAction>
	{
	public:
		explicit CommandPlayerAptMe(CommandList* const parent)
			: CommandWithOnTickFocused(parent, LOC("NEAR_ME"), CMDNAMES("aptme"), NOLABEL, 0, COMMANDPERM_RUDE)
		{
			PC_PTR->registerCommand(this);
		}

		void onTickFocused() final
		{
			const auto ped_pos = g_player_ped.getPos();
			DrawUtil3d::draw_line_and_ar_beacon(ped_pos, AptHelper::getNearestPos(ped_pos));
		}

		void onClick(Click& click) final
		{
			ensureScriptThread(click, [this, click]
			{
				DEF_P2;
				const auto players = pp->getPlayers(pp->single);
				const uint8_t pos_id = AptHelper::getNearestPosId(click.getEffectiveIssuer().getPos());
				if (!is_session_started())
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
	};
}
