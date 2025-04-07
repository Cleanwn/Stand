#include "gta_vehicle_layout.hpp"

#include "CDrivebyWeaponGroup.hpp"
#include "DrivebyCommons.hpp"

bool CVehicleSeatAnimInfo::allowsWeapon(const Stand::Weapon& w) const
{
	if (m_DriveByInfo
		&& !Stand::DrivebyCommons::isFakeDrivebyInfo(&m_DriveByInfo)
		)
	{
		for (const auto& driveByAnimInfo : m_DriveByInfo->m_DriveByAnimInfos)
		{
			if (driveByAnimInfo
				&& driveByAnimInfo->m_WeaponGroup
				&& driveByAnimInfo->m_WeaponGroup->allowsWeapon(w)
				)
			{
				return true;
			}
		}
	}
	return false;
}

bool CVehicleLayoutInfo::doesSeatAllowWeapon(const Stand::Weapon& w, uint16_t seat) const
{
	if (seat >= 0 && seat < m_Seats.size())
	{
		if (auto info = m_Seats[seat].m_SeatAnimInfo)
		{
			return info->allowsWeapon(w);
		}
	}

	return false;
}

bool CVehicleLayoutInfo::doesAnySeatAllowWeapon(const Stand::Weapon& w) const
{
	for (const auto& seat : m_Seats)
	{
		if (seat.m_SeatAnimInfo
			&& seat.m_SeatAnimInfo->allowsWeapon(w)
			)
		{
			return true;
		}
	}
	return false;
}
