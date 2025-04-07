#include "CommandSpecialFlight.hpp"

#include "AbstractEntity.hpp"
#include "CHandlingData.hpp"
#include "CHandlingDataMgr.hpp"
#include "ComponentCrashPatch.hpp"
#include "gta_vehicle.hpp"
#include "pointers.hpp"
#include "regular_event.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandSpecialFlight::CommandSpecialFlight(CommandList* const parent)
		: CommandListSelect(parent, LOC("HNE_SPECFLY"), {}, LOC("SPECFLY_H"), {
			{0, LOC("OVRDOFF")},
			{1, LOC("DELUXO")},
			{2, LOC("OPP2")},
		}, 0)
	{
	}

	static std::vector<CHandlingData*> modified_handling_data{};

	template <hash_t hash>
	static void tickHandler()
	{
		if (auto v = g_player_veh.getCVehicle())
		{
			if (v->handling_data)
			{
				if (!v->handling_data->hasSubhandlingData(HANDLING_TYPE_SPECIAL_FLIGHT)
					&& v->num_wheels >= 2
					)
				{
					v->handling_data->sub_handling_arr.emplace_back(pointers::handling_data_mgr->getHandlingDataByNameHash(hash)->getSubhandlingData(HANDLING_TYPE_SPECIAL_FLIGHT));
					modified_handling_data.emplace_back(v->handling_data);
				}
			}
		}
	}

	void CommandSpecialFlight::onChange(Click& click, long long prev_value)
	{
		if (transitioning)
		{
			if (!click.isAuto())
			{
				auto derived = click.deriveAuto();
				setValue(derived, prev_value);
			}
			return;
		}
		transitioning = true;
		ensureYieldableScriptThread(click, [this, prev_value]
		{
			// Unregister old tick handler
			switch (prev_value)
			{
			case 1:
				reScriptTickEvent::unregisterHandler(&tickHandler<ATSTRINGHASH("deluxo")>);
				break;

			case 2:
				reScriptTickEvent::unregisterHandler(&tickHandler<ATSTRINGHASH("oppressor2")>);
				break;
			}

			// Handle disabling elegantly
			if (value == 0)
			{
				if (auto v = g_player_veh.getCVehicle())
				{
					// Ensure we can toggle out of flight mode
					if (prev_value == 2)
					{
						cleanupHandlingData();
						reScriptTickEvent::registerHandler(&tickHandler<ATSTRINGHASH("deluxo")>);
						Script::current()->yield();
						reScriptTickEvent::unregisterHandler(&tickHandler<ATSTRINGHASH("deluxo")>);
					}

					// And toggle out of it
					if (v->m_specialFlightModeRatio != 0.0f)
					{
						VEHICLE::SET_SPECIAL_FLIGHT_MODE_TARGET_RATIO(g_player_veh, 0.0f);
						Script::current()->yield(1000);
					}
				}
			}

			// Undo the damage we've done
			cleanupHandlingData();

			// Register new tick handler
			switch (value)
			{
			case 1:
				reScriptTickEvent::registerHandler(&tickHandler<ATSTRINGHASH("deluxo")>);
				break;

			case 2:
				reScriptTickEvent::registerHandler(&tickHandler<ATSTRINGHASH("oppressor2")>);
				break;
			}

			// Update if invalid mechanical part should be raised to the user
			g_comp_crashpatch.silently_block_invalid_mechanical_part = (value != 0);

			transitioning = false;
		});
	}

	void CommandSpecialFlight::cleanupHandlingData()
	{
		if (!modified_handling_data.empty())
		{
			for (auto& hd : modified_handling_data)
			{
				if (auto shd = hd->getSubhandlingData(HANDLING_TYPE_SPECIAL_FLIGHT))
				{
					hd->sub_handling_arr.Delete(hd->sub_handling_arr.Find(shd));
				}
			}
			modified_handling_data.clear();
		}
	}
}
