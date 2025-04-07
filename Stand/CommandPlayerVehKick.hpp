#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "AbstractPlayer.hpp"
#include "gta_net_object_mgr.hpp"
#include "gta_vehicle.hpp"
#include "PlayerProvider.hpp"
#include "pointers.hpp"
#include "tse.hpp"

namespace Stand
{
	class CommandPlayerVehKick : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerVehKick(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("VEHKICK"), { CMDNAME("vehkick") })
		{
		}

		void onClick(Click& click) final
		{
			PC_PTR->ensureOnlineAndWarnSelfharm(this, click, [this](ThreadContext thread_context)
			{
				ensureScriptThread(thread_context, [this]
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						if (auto veh = p.getVehicle(); veh.isValid())
						{
							if (DECORATOR::DECOR_GET_INT(veh, "Player_Vehicle") == NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(p))
							{
								auto handle = veh.getHandle();
								ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&handle);
								continue;
							}
						}

						// SET_VEHICLE_EXCLUSIVE_DRIVER can also be abused

						// This works, but it causes lots of syncing problems.
						/*if (auto v = p.getVehicle().getCVehicle())
						{
							if (v->m_net_object)
							{
								(*pointers::network_object_mgr)->changeOwner(v->m_net_object, g_player.getCNetGamePlayer(), 0);
							}
						}*/

						uint64_t args[] = {
							Util::i32_to_u64_trickery(SE_PV_KICK),
							g_player,
							0, // 2
							0, // 3
							0, // 4
							0, // 5
							1, // 6
							p, // 7
						};
						TSE(1 << p, args);
					}
				});
			});
		}
	};
}
