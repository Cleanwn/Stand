#pragma once

#include "CommandToggle.hpp"

#include "pointers.hpp"
#include "PopMultiplierAreas.hpp"

//#include "drawDebugText.hpp"

namespace Stand
{
	struct CommandNoModPop : public CommandToggle
	{
		explicit CommandNoModPop(CommandList* parent)
			: CommandToggle(parent, LOC("NOMODPOP"), combineCommandNames(CMDNAMES("no", "block", "disable"), CMDNAMES_OBF("modpop")), LOC("NOMODPOP_H"))
		{
		}

		void onChange(Click& click) final
		{
			onChangeToggleScriptTickEventHandler(click, []
			{
#if false
				// CNetworkPopMultiplierAreaWorldStateData::_ms_pPool
				auto area_states = *(rage::fwBasePool**)IDA_ADDR(00007FF7DCBC2C50);
				for (int i = 0; i != area_states->m_nSize; ++i)
				{
					drawDebugText(area_states->GetAt(i));
				}
#endif

				for (int i = 0; i != MAX_POP_MULTIPLIER_AREAS; ++i)
				{
					PopMultiplierArea& area = pointers::pop_multiplier_areas[i];
					if (!area.m_init)
					{
						continue;
					}
					/*if (area.m_isSphere)
					{
						drawDebugText(area.GetRadius());
						drawDebugText(area.m_pedDensityMultiplier);
						drawDebugText(area.m_trafficDensityMultiplier);
					}*/
					if (area.m_isSphere)
					{
						const bool is_modded = (
							(area.GetRadius() != 0.0f // Exiting interiors adds a 0-radius one for a short time.
							&& (area.GetRadius() < 0.00001f // Seen a modded area use a really small number between 0.0f and 1.0f but it affected the entire world.
								|| area.GetRadius() > 9999.0f
								)
							)
						);
#ifdef STAND_DEV
						if (is_modded)
						{
							Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Deleted a modded population multiplier area ({}, {}, {})").str()), area.GetRadius(), area.m_pedDensityMultiplier, area.m_trafficDensityMultiplier));
						}
						else
						{
							Util::toast(fmt::format(fmt::runtime(soup::ObfusString("Deleted a population multiplier area ({}, {}, {})").str()), area.GetRadius(), area.m_pedDensityMultiplier, area.m_trafficDensityMultiplier));
						}
#else
						if (is_modded)
						{
							Util::toast(LOC("NOMODPOP_T"));
						}
#endif
						MISC::REMOVE_POP_MULTIPLIER_AREA(i, TRUE);
					}
				}
			});
		}
	};
}
