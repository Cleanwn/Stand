#include "WeaponMgr.hpp"

#include "AbstractEntity.hpp"
#include "ExecCtx.hpp"
#include "gta_ped.hpp"
#include "gta_vehicle.hpp"
#include "Gui.hpp"
#include "pointers.hpp"
#include "weapons.hpp"

namespace Stand
{
	static std::unordered_map<hash_t, float> weapon_bullet_damage_map = {};

	static std::unordered_map<hash_t, float> weapon_lockon_range_map = {};
	static std::unordered_map<hash_t, float> weapon_range_map = {};

	static std::unordered_map<hash_t, float> weapon_spin_map = {};
	static std::unordered_map<hash_t, float> weapon_spinup_map = {};
	static std::unordered_map<hash_t, float> weapon_spindown_map = {};

	static std::unordered_map<hash_t, float> weapon_spread_map = {};
	static std::unordered_map<hash_t, float> weapon_batch_spread_map = {};

	static std::unordered_map<hash_t, float> weapon_recoil_amp_map = {};
	static std::unordered_map<hash_t, float> weapon_explosion_amp_map = {};

	static std::unordered_map<hash_t, unsigned int> weapon_recoil_shake_map = {};

	static std::unordered_map<hash_t, hash_t> weapon_recoil_shake_hash_map = {};
	static std::unordered_map<hash_t, hash_t> weapon_recoil_shake_hash_first_person_map = {};

	static std::unordered_map<hash_t, float> lockon_speed_map = {};

	static std::unordered_map<hash_t, float> launch_speed_map = {};

	static std::unordered_map<hash_t, float> m_ReloadTimeSPVehicle = {};
	static std::unordered_map<hash_t, float> m_ReloadTimeMPVehicle = {};
	static std::unordered_map<hash_t, float> m_AnimReloadRateVehicle = {};
	static std::unordered_map<hash_t, float> m_TimeBetweenShotsVehicle = {};
	static std::unordered_map<hash_t, float> m_AlternativeWaitTimeVehicle = {};

	static bool og_need_reset = false;

	template <typename T>
	static void applyWeaponStatMultiplier(const hash_t model_hash, T& stat, std::unordered_map<hash_t, T>& map, const T multiplier)
	{
		const auto i = map.find(model_hash);
		if (g_gui.isUnloadPending() || multiplier == 1)
		{
			if (i != map.end())
			{
				stat = i->second;
				map.erase(i);
			}
		}
		else
		{
			if (i == map.end())
			{
				map.emplace(model_hash, stat);
				stat *= multiplier;
			}
			else
			{
				stat = i->second * multiplier;
			}
		}
	}

	void WeaponMgr::applyWeaponStatMultipliers(CWeaponInfo* weapon_info)
	{
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_LockOnRange, weapon_lockon_range_map, weapon_range_multiplier);
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_WeaponRange, weapon_range_map, weapon_range_multiplier);

		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_SpinTime, weapon_spin_map, weapon_spool_delay_multiplier);
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_SpinUpTime, weapon_spinup_map, weapon_spool_delay_multiplier);
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_SpinDownTime, weapon_spindown_map, weapon_spool_delay_multiplier);

		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_AccuracySpread, weapon_spread_map, weapon_spread_multiplier);
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_BatchSpread, weapon_batch_spread_map, weapon_spread_multiplier);

		if (auto weapon = Weapon::find(weapon_info->name_hash); weapon && weapon->category == Weapon::SHOTGUN)
		{
			weapon_info->m_BulletsInBatch = weapon_bullets_in_batch == 0 ? weapon->bullets_in_batch : weapon_bullets_in_batch;
		}

		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_RecoilShakeAmplitude, weapon_recoil_amp_map, weapon_recoil_multiplier);
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_ExplosionShakeAmplitude, weapon_explosion_amp_map, weapon_recoil_multiplier);

		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_MinTimeBetweenRecoilShakes, weapon_recoil_shake_map, weapon_recoil_shake_multiplier);

		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_RecoilShakeHash, weapon_recoil_shake_hash_map, weapon_recoil_shake_hash_identity);
		applyWeaponStatMultiplier(weapon_info->name_hash, weapon_info->m_RecoilShakeHashFirstPerson, weapon_recoil_shake_hash_first_person_map, weapon_recoil_shake_hash_identity);

		if (auto ammo_info = static_cast<CAmmoRocketInfo*>(weapon_info->m_AmmoInfo))
		{
			if (ammo_info->getNameHash() == ATSTRINGHASH("CAmmoRocketInfo"))
			{
				applyWeaponStatMultiplier(weapon_info->name_hash, ammo_info->m_TimeBeforeHoming, lockon_speed_map, lockon_speed_multiplier);
				applyWeaponStatMultiplier(weapon_info->name_hash, ammo_info->m_LaunchSpeed, launch_speed_map, rocket_speed_multiplier);
			}
		}
	}

	void WeaponMgr::applyVehicleWeaponStatMultipliers(CWeaponInfo* vehicle_weapon)
	{
		applyWeaponStatMultiplier(vehicle_weapon->name_hash, vehicle_weapon->m_ReloadTimeSP, m_ReloadTimeSPVehicle, vehicle_reload_time);
		applyWeaponStatMultiplier(vehicle_weapon->name_hash, vehicle_weapon->m_ReloadTimeMP, m_ReloadTimeMPVehicle, vehicle_reload_time);
		applyWeaponStatMultiplier(vehicle_weapon->name_hash, vehicle_weapon->m_AnimReloadRate, m_AnimReloadRateVehicle, vehicle_reload_time);
		applyWeaponStatMultiplier(vehicle_weapon->name_hash, vehicle_weapon->m_TimeBetweenShots, m_TimeBetweenShotsVehicle, vehicle_reload_time);
		applyWeaponStatMultiplier(vehicle_weapon->name_hash, vehicle_weapon->m_AlternateWaitTime, m_AlternativeWaitTimeVehicle, vehicle_reload_time);

		applyWeaponStatMultipliers(vehicle_weapon);

		if (vehicle_reload_time != 1.0f)
		{
			og_need_reset = true;
			*pointers::balanced_cannons_time_between_shots = 0.0f;
			*pointers::balanced_cannons_alternate_wait_time = 0.0f;
		}
		else if (og_need_reset)
		{
			og_need_reset = false;
			*pointers::balanced_cannons_time_between_shots = 0.125f;
			*pointers::balanced_cannons_alternate_wait_time = 0.125f;
		}
	}

	void WeaponMgr::onTick()
	{
		if (auto* const ped = g_player_ped.getCPed())
		{
			if (CPedWeaponManager* const weapon_manager = ped->weapon_manager)
			{
				if (CWeaponInfo* const current_weapon = weapon_manager->equippedWeapon.equippedWeaponInfo)
				{
					applyWeaponStatMultiplier(current_weapon->name_hash, current_weapon->m_Damage, weapon_bullet_damage_map, weapon_bullet_damage_multiplier);
					applyWeaponStatMultipliers(current_weapon);
				}
				if (CWeaponInfo* vehicle_weapon = weapon_manager->equippedWeapon.equippedVehicleWeaponInfo)
				{
					SOUP_ASSERT(ExecCtx::get().isScript());
					{
						// The game does not properly re-equip weapons when changing our vehicle mods forces our equipped vehicle weapon to change.
						// This breaks WeaponMgr for obvious reasons, we're changing the CWeaponInfo of a weapon we aren't using (until we get out and back into the vehicle).
						// But it also breaks weapon animation and audio streaming, which is why we convince the game to fix these incorrect values instead of changing what value we refer to.
						//
						// SET_CURRENT_PED_VEHICLE_WEAPON -> CPedWeaponManager::EquipWeapon -> CPedEquippedWeapon::SetVehicleWeaponInfo (m_equippedWeapon).

						Hash weapon = 0;
						if (WEAPON::GET_CURRENT_PED_VEHICLE_WEAPON(g_player_ped, &weapon); weapon != 0 && weapon != vehicle_weapon->name_hash)
						{
							WEAPON::SET_CURRENT_PED_VEHICLE_WEAPON(g_player_ped, weapon);
							vehicle_weapon = weapon_manager->equippedWeapon.equippedVehicleWeaponInfo;
						}
					}
					applyVehicleWeaponStatMultipliers(vehicle_weapon);
				}
			}
		}
	}
}
