#pragma once

#include "CommandToggle.hpp"

#include "gta_net_object_mgr.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandNoBlood : public CommandToggle
	{
	private:
		time_t last_synch = 0;

	public:
		explicit CommandNoBlood(CommandList* const parent)
			: CommandToggle(parent, LOC("NOBLD"), { CMDNAME("noblood") }, LOC("NOBLD_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
				// If we were to allow this to synch every tick, CPedGameStateDataNode would be permanently dirty,
				// which would prevent vehicle control migration because m_PedInSeatStateFullySynced would permanently be false.
				if (auto obj = g_player_ped.getNetObject())
				{
					const auto prev = static_cast<CNetObjPed*>(obj)->m_ClearDamageCount;
					PED::CLEAR_PED_BLOOD_DAMAGE(g_player_ped);
					if (GET_MILLIS_SINCE(last_synch) > 10000)
					{
						last_synch = get_current_time_millis();
					}
					else
					{
						static_cast<CNetObjPed*>(obj)->m_ClearDamageCount = prev;
					}
				}
				else
				{
					PED::CLEAR_PED_BLOOD_DAMAGE(g_player_ped);
				}
				HANDLER_END;
			});
		}
	};
}
