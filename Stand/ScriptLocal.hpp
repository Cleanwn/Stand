#pragma once

#include <cstddef>
#include <type_traits>
#include <Windows.h>

#include "gta_fwddecl.hpp"

// fm_content_business_battles

#define LOCAL_BB2_SHDATA			5257
#define LOCAL_BB2_STATE				LOCAL_BB2_SHDATA + 1137

// casinoroulette

#define LOCAL_CR_BASE				124 // NETWORK_REGISTER_HOST_BROADCAST_VARIABLES
#define LOCAL_CR_SPINRESULT			1357 + 153 // if (Var0.f_4 == 31)

// am_mp_drone
#define LOCAL_DRONE_DATA 199
#define LOCAL_DRONE_SHOCK_COOLDOWN LOCAL_DRONE_DATA + 249 // = 100;
#define LOCAL_DRONE_BOOST_RECHARGE_COOLDOWN LOCAL_DRONE_DATA + 253 // GET_TIME_DIFFERENCE
#define LOCAL_DRONE_DETONATE_COOLDOWN LOCAL_DRONE_DATA + 348

#pragma region ScrUpdate
// maintransition
#define LOCAL_MT_LFF 152
#define LOCAL_MT_LFF_CHECKIDX 352
// am_pi_menu
#define LOCAL_PI_STATE 1526
#define LOCAL_PI_BITSET 1692
// am_launcher
#define LOCAL_LNCH_PLAYERSTATE 238
// am_penned_in
#define LOCAL_PNDIN_SHDATA 98
// am_cp_collection
#define LOCAL_CPCOL_SHDATA 820
// am_hunt_the_beast
#define LOCAL_HTB_SHDATA 604
#define LOCAL_HTB_BEAST LOCAL_HTB_SHDATA + 1 + 6
// am_challenges
#define LOCAL_CHLNG_SHDATA 642
#define LOCAL_SHDATA_WARMUP 43
#define LOCAL_CHLNG_PLYDATA 2609
#define LOCAL_CHLNG_PLYDATA_SIZE 12
// am_criminal_damage
#define LOCAL_CRIMDMG_SCORE 110
#define LOCAL_CRIMDMG_SHDATA 115
#define LOCAL_CRIMDMG_PLYDATA 2047
#define LOCAL_CRIMDMG_PLYDATA_SIZE 7
// business_battles
#define LOCAL_BB_SHDATA 2404
// shop_controller
#define LOCAL_SHOPCTRL_CHEAT 337
#define LOCAL_SHOPCTRL_CHEAT_STATE LOCAL_SHOPCTRL_CHEAT + 3
// blackjack
#define LOCAL_BJ_DEALERCARDS_1 116
#define LOCAL_BJ_DEALERCARDS_2 846
#define LOCAL_BJ_DEALERCARDS LOCAL_BJ_DEALERCARDS_1 + LOCAL_BJ_DEALERCARDS_2
#define LOCAL_BJ_DEALER_SIZE 13
// am_gang_call
#define LOCAL_GANGCALL_SHDATA 62
// am_mp_property_int
#define LOCAL_INT_SHDATA 10566
#define LOCAL_INT_SHDATA_KICK 677
#pragma endregion ScrUpdate

#define LOCAL_GANGCALL_STATE		0
#define LOCAL_GANGCALL_BITSET		1
#define LOCAL_GANGCALL_GANGMEMBER	10
#define LOCAL_GANGMEMBER_SIZE		7

namespace Stand
{
	class ScriptLocal
	{
	public:
		explicit ScriptLocal(rage::scrThread* thread, std::size_t index);
		explicit ScriptLocal(PVOID stack, std::size_t index);

		ScriptLocal at(std::ptrdiff_t index);
		ScriptLocal at(std::ptrdiff_t index, std::size_t size);

		template <typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> as()
		{
			return static_cast<T>(get());
		}

		template <typename T>
		std::enable_if_t<std::is_lvalue_reference_v<T>, T> as()
		{
			return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(get());
		}
	private:
		void* get();
		std::size_t m_index;
		PVOID m_stack;
	};
}
