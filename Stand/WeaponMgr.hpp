#pragma once

#include "gta_fwddecl.hpp"
#include "natives_decl.hpp"

#include <unordered_map>

namespace Stand
{
	class WeaponMgr
	{
	public:
		inline static float weapon_bullet_damage_multiplier = 1.0f;
		inline static float weapon_range_multiplier = 1.0f;
		inline static float lockon_speed_multiplier = 1.0f;
		inline static float weapon_spool_delay_multiplier = 1.0f;
		inline static float weapon_spread_multiplier = 1.0f;
		inline static float weapon_recoil_multiplier = 1.0f;
		inline static unsigned int weapon_recoil_shake_multiplier = 1;
		inline static hash_t weapon_recoil_shake_hash_identity = 1;
		inline static float rocket_speed_multiplier = 1.0f;
		inline static unsigned int weapon_bullets_in_batch = 0;
		inline static float vehicle_reload_time = 1.0f;

	private:
		static void applyWeaponStatMultipliers(CWeaponInfo* weapon_info);
		static void applyVehicleWeaponStatMultipliers(CWeaponInfo* weapon_info);
	public:
		static void onTick();
	};
}
