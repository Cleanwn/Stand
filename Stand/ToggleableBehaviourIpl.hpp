#pragma once

#include "tbToggleableBehaviour.hpp"

#include <vector>

#include "atStringHash.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct DoorData
	{
		hash_t door_hash;
		hash_t model_hash;
		float x;
		float y;
		float z;
	};

	struct IplData
	{
		const std::vector<const char*> request{};
		const std::vector<const char*> remove{};
		const std::vector<const char*> remove_enable_only{};
		const std::vector<DoorData> doors{};

		void enable() const;
		void tick() const;
		void disable() const;
	};

	class ToggleableBehaviourIpl : public tbToggleableBehaviour
	{
	public:
		const IplData data;

		explicit constexpr ToggleableBehaviourIpl(IplData&& data) noexcept
			: data(std::move(data))
		{
		}

	protected:
		void onEnable() final;
	};
#pragma pack(pop)

	inline auto g_ipl_morgue = ToggleableBehaviourIpl({ {"Coroner_Int_on", "coronertrash"} });
	inline auto g_ipl_hospital = ToggleableBehaviourIpl({ {"RC12B_HospitalInterior"}, {"RC12B_Default"}, {"RC12B_Fixed", "RC12B_Destroyed"} });
	inline auto g_ipl_lost_mc_clubhouse = ToggleableBehaviourIpl({ {"bkr_bi_hw1_13_int"}, {"hei_bi_hw1_13_door"}});
	inline auto g_ipl_fameorshame = ToggleableBehaviourIpl({ {"sp1_10_real_interior"}, {"sp1_10_fake_interior"} });
	inline auto g_ipl_jewel = ToggleableBehaviourIpl({ {"post_hiest_unload"}, {"jewel2fake"}, {"bh1_16_refurb"}, {
		{1874948872, ATSTRINGHASH("p_jewel_door_l"), -631.96f, -236.33f, 38.21f},
		{-1965020851, ATSTRINGHASH("p_jewel_door_r1"), -630.43f, -238.44f, 38.21f}
	} });
	inline auto g_ipl_max_renda = ToggleableBehaviourIpl({ {"refit_unload"}, {"bh1_16_doors_shut"} });
	inline auto g_ipl_fib_lobby = ToggleableBehaviourIpl({ {"FIBlobby"}, {"FIBlobbyfake"}, {}, {
		{756894459, ATSTRINGHASH("v_ilev_fibl_door02"), 106.38f, -742.7f, 46.18f},
		{476981677, ATSTRINGHASH("v_ilev_fibl_door01"), 105.76f, -746.65f, 46.18f},
	} });
	inline auto g_ipl_union_dep = ToggleableBehaviourIpl({ {"FINBANK"} });
	inline auto g_ipl_lifeinvader = ToggleableBehaviourIpl({ {"facelobby"}, {"facelobbyfake"}, {}, {
		{0, ATSTRINGHASH("v_ilev_fb_door01"), -1083.62f, -260.417f, 38.187f},
		{0, ATSTRINGHASH("v_ilev_fb_door02"), -1080.974f, -259.02f, 38.187f},
		{0, ATSTRINGHASH("v_ilev_fb_doorshortl"), -1045.12f, -232.004f, 39.438f},
		{0, ATSTRINGHASH("v_ilev_fb_doorshortr"), -1046.516f, -229.358f, 39.438f},
		{0, ATSTRINGHASH("v_ilev_door_orange"), -1063.804f, -240.832f, 39.883f},
		{0, ATSTRINGHASH("v_ilev_door_orangesolid"), -1055.958f, -236.425f, 44.171f}
	} });
}
