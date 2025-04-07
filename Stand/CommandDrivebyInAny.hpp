#pragma once

#include "CommandToggle.hpp"

#include "CVehicleModelInfo.hpp"
#include "gta_vehicle_layout.hpp"
#include "CVehicleMetadataMgr.hpp"
#include "DrivebyCommons.hpp"

namespace Stand
{
	class CommandDrivebyInAny : public CommandToggle
	{
	public:
		explicit CommandDrivebyInAny(CommandList* const parent)
			: CommandToggle(parent, LOC("DRBYINANY"), CMDNAMES("drivebyinall", "drivebyinany", "drivebyallvehicles"))
		{
		}

		void cleanup()
		{
			if (!DrivebyCommons::fake_driveby_infos.empty())
			{
				for (const auto& p : DrivebyCommons::fake_driveby_infos)
				{
					*p = nullptr;
				}
				DrivebyCommons::fake_driveby_infos.clear();
			}
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on)
				if (auto veh = g_player_veh.getCVehicle())
				{
					for (auto& seat : reinterpret_cast<CVehicleModelInfo*>(veh->archetype)->m_pVehicleLayoutInfo->m_Seats)
					{
						if (seat.m_SeatAnimInfo->m_DriveByInfo == nullptr)
						{
							DrivebyCommons::fake_driveby_infos.emplace_back(&seat.m_SeatAnimInfo->m_DriveByInfo);

							for (const auto& driveby_info : pointers::vehicle_metadata->driveby_infos)
							{
								if (driveby_info->m_Name == ATSTRINGHASH("DRIVEBY_STANDARD_FRONT_LEFT"))
								{
									seat.m_SeatAnimInfo->m_DriveByInfo = driveby_info;
									break;
								}
							}
						}
					}
				}
				else
				{
					cleanup();
				}
				HANDLER_END;
			});
		}

		void onDisable(Click& click) final
		{
			cleanup();
		}
	};
}
