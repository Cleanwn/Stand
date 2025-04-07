#include "gta_ped.hpp"

#include "CObject.hpp"

const CWeapon* CPedWeaponManager::GetEquippedWeapon() const
{
	if (equippedWeapon.object)
	{
		return equippedWeapon.object->m_pWeapon;
	}
	// There is some more logic here, but I don't think it matters for our needs.
	return nullptr;
}
