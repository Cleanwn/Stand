#include "AbstractModel.hpp"

#include "CBaseModelInfo.hpp"
#include "CVehicleModelInfo.hpp"
#include "get_vehicle_display_name.hpp"
#include "Hooking.hpp"
#include "atStringHash.hpp"
#include "joaatToString.hpp"
#include "natives.hpp"
#include "PedModel.hpp"
#include "Util.hpp"
#include "VehicleColour.hpp"

namespace Stand
{
	bool AbstractModel::isValidModel() const
	{
		return STREAMING::IS_MODEL_VALID(model);
	}

	bool AbstractModel::isNull() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("slod_human"):
		case ATSTRINGHASH("slod_large_quadped"):
		case ATSTRINGHASH("slod_small_quadped"):
			return true;
		}
		return isNullVehicle();
	}

	bool AbstractModel::isNullVehicle() const noexcept
	{
		// [[nextgen]]
		switch (model)
		{
		case ATSTRINGHASH("arbitergt"):
		case ATSTRINGHASH("astron2"):
		case ATSTRINGHASH("cyclone2"):
		case ATSTRINGHASH("ignus2"):
		case ATSTRINGHASH("s95"):
			return true;
		}
		return false;
	}

	CBaseModelInfo* AbstractModel::getModelInfo() const noexcept
	{
		return g_hooking.getModelInfo(model);
	}

	CVehicleModelInfo* AbstractModel::getVehicleModelInfo() const noexcept
	{
		if (auto mi = getModelInfo())
		{
			if (mi->GetModelType() == MI_TYPE_VEHICLE)
			{
				return reinterpret_cast<CVehicleModelInfo*>(mi);
			}
		}
		return nullptr;
	}

	bool AbstractModel::isPed() const noexcept
	{
		auto info = getModelInfo();
		return info && info->m_type == MI_TYPE_PED;
	}

	bool AbstractModel::isObject() const noexcept
	{
		auto info = getModelInfo();
		if (info == nullptr)
		{
			return true;
		}
		auto type = info->GetModelType();
		return type == MI_TYPE_BASE
			|| type == MI_TYPE_WEAPON
			;
	}

	bool AbstractModel::isObjectStrict() const noexcept
	{
		if (auto info = getModelInfo())
		{
			auto type = info->GetModelType();
			return type == MI_TYPE_BASE
				|| type == MI_TYPE_WEAPON
				;
		}
		return false;
	}

	bool AbstractModel::isVehicle() const noexcept
	{
		auto info = getModelInfo();
		return info && info->m_type == MI_TYPE_VEHICLE;
	}

	Vector3 AbstractModel::getDimensions() const
	{
		Vector3 min;
		Vector3 dimensions;
		MISC::GET_MODEL_DIMENSIONS(model, &min, &dimensions);
		dimensions -= min;
		return dimensions;
	}

	Label AbstractModel::getLabel() const
	{
		if (STREAMING::IS_MODEL_A_PED(model))
		{
			if (auto ped = PedModel::fromHash(model))
			{
				return ped->menu_name;
			}
		}
		else if (STREAMING::IS_MODEL_A_VEHICLE(model))
		{
			if (auto vehicle_item = VehicleItem::fromHash(model))
			{
				return LIT(get_vehicle_display_name_no_manufacturer(vehicle_item));
			}
		}
		else
		{
			if (auto obj = joaatToStringRaw(model))
			{
				return LIT(obj);
			}
		}
		return LIT(Util::to_padded_hex_string_with_0x(model));
	}

	Label AbstractModel::getLabelPed() const
	{
		if (auto ped = PedModel::fromHash(model))
		{
			return ped->menu_name;
		}
		return LIT(Util::to_padded_hex_string_with_0x(model));
	}

	VehicleType AbstractModel::getVehicleType() const noexcept
	{
		if (auto info = getVehicleModelInfo())
		{
			return info->vehicle_type;
		}
		return (VehicleType)-1;
	}

	bool AbstractModel::isSubmarine() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_SUBMARINE;
	}

	bool AbstractModel::isTrailer() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_TRAILER;
	}

	bool AbstractModel::isPlane() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_PLANE;
	}

	bool AbstractModel::isHeli() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_HELI;
	}

	bool AbstractModel::isBike() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_BIKE;
	}

	bool AbstractModel::isBoat() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_BOAT;
	}

	bool AbstractModel::isBicycle() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_BICYCLE;
	}

	bool AbstractModel::isSubmarineCar() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_SUBMARINECAR;
	}

	bool AbstractModel::isTrain() const noexcept
	{
		return getVehicleType() == VEHICLE_TYPE_TRAIN;
	}

	bool AbstractModel::isAirborne() const noexcept
	{
		switch (getVehicleType())
		{
		case VEHICLE_TYPE_HELI:
		case VEHICLE_TYPE_BLIMP:
		case VEHICLE_TYPE_PLANE:
			return true;
		}
		return false;
	}

	bool AbstractModel::isAquatic() const noexcept
	{
		switch (getVehicleType())
		{
		case VEHICLE_TYPE_BOAT:
		case VEHICLE_TYPE_SUBMARINE:
			return true;
		}
		return false;
	}

	int AbstractModel::getNumSeats() const
	{
		// GET_VEHICLE_MODEL_NUMBER_OF_SEATS returns the wrong info for these models when they're not streamed in
		switch (model)
		{
		case ATSTRINGHASH("cheburek"): return 4;
		case ATSTRINGHASH("issi8"): return 4;
		case ATSTRINGHASH("kamacho"): return 4;
		case ATSTRINGHASH("rhinehart"): return 4;
		case ATSTRINGHASH("seminole2"): return 4;
		case ATSTRINGHASH("stafford"): return 4;
		case ATSTRINGHASH("sultan2"): return 4;
		case ATSTRINGHASH("trailerlarge"): return 3;
		}
		return VEHICLE::GET_VEHICLE_MODEL_NUMBER_OF_SEATS(model);
	}

	bool AbstractModel::isTank() const noexcept
	{
		return model == ATSTRINGHASH("rhino") || model == ATSTRINGHASH("khanjali");
	}

	bool AbstractModel::isTraffic() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("radi"):
		case ATSTRINGHASH("intruder"):
		case ATSTRINGHASH("granger"):
		case ATSTRINGHASH("cavalcade2"):
		case ATSTRINGHASH("premier"):
		case ATSTRINGHASH("pcj"):
		case ATSTRINGHASH("akuma"):
		case ATSTRINGHASH("futo"):
		case ATSTRINGHASH("minivan"):
		case ATSTRINGHASH("rancherxl"):
		case ATSTRINGHASH("buccaneer"):
		case ATSTRINGHASH("bullet"):
		case ATSTRINGHASH("bagger"):
		case ATSTRINGHASH("felon2"):
		case ATSTRINGHASH("baller2"):
		case ATSTRINGHASH("feltzer2"):
		case ATSTRINGHASH("ruiner"):
		case ATSTRINGHASH("carbonizzare"):
		case ATSTRINGHASH("schafter2"):
		case ATSTRINGHASH("serrano"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isTowTruck() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("towtruck"):
		case ATSTRINGHASH("towtruck2"):
		case ATSTRINGHASH("towtruck3"):
		case ATSTRINGHASH("towtruck4"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isCargobob() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("cargobob"):
		case ATSTRINGHASH("cargobob2"):
		case ATSTRINGHASH("cargobob3"):
		case ATSTRINGHASH("cargobob4"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isBigVehicle() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("alkonost"):
		case ATSTRINGHASH("avenger2"):
		case ATSTRINGHASH("dump"):
		case ATSTRINGHASH("blimp"):
		case ATSTRINGHASH("blimp2"):
		case ATSTRINGHASH("blimp3"):
		case ATSTRINGHASH("bombushka"):
		case ATSTRINGHASH("cargoplane"):
		case ATSTRINGHASH("cargoplane2"):
		case ATSTRINGHASH("hydra"):
		case ATSTRINGHASH("jet"):
		case ATSTRINGHASH("tug"):
		case ATSTRINGHASH("volatol"):

		case ATSTRINGHASH("tr2"):
		case ATSTRINGHASH("tr3"):
		case ATSTRINGHASH("tr4"):
			return true;
		}
		return false;
	}

	bool AbstractModel::hasSpecialFlight() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("deluxo"):
		case ATSTRINGHASH("oppressor2"):
			return true;
		}
		return false;
	}

	bool AbstractModel::hasVtol() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("hydra"):
		case ATSTRINGHASH("tula"):
		case ATSTRINGHASH("avenger"):
		case ATSTRINGHASH("avenger2"):
		case ATSTRINGHASH("avenger3"):
		case ATSTRINGHASH("avenger4"):
		case ATSTRINGHASH("raiju"):
			return true;
		}
		return false;
	}

	bool AbstractModel::hasDriftTune() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("drifteuros"):
		case ATSTRINGHASH("driftzr350"):
		case ATSTRINGHASH("driftfr36"):
		case ATSTRINGHASH("driftfuto"):
		case ATSTRINGHASH("driftjester"):
		case ATSTRINGHASH("driftyosemite"):
		case ATSTRINGHASH("driftremus"):
		case ATSTRINGHASH("drifttampa"):
			return true;
		}
		return false;
	}

	bool AbstractModel::hasWindowTintMinusOne() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("granger"):
		case ATSTRINGHASH("visione"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isStealthVehicle() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("akula"):
		case ATSTRINGHASH("alkonost"):
		case ATSTRINGHASH("annihilator2"):
		case ATSTRINGHASH("kosatka"):
		case ATSTRINGHASH("microlight"):
		case ATSTRINGHASH("raiju"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isRemoteControllable() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("rcbandito"):
		case ATSTRINGHASH("minitank"):
			return true;
		}
		return isRemoteControllablePv();
	}

	bool AbstractModel::isRemoteControllablePv() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("deity"):
		case ATSTRINGHASH("granger2"):
		case ATSTRINGHASH("buffalo4"):
		case ATSTRINGHASH("jubilee"):
		case ATSTRINGHASH("patriot3"):
		case ATSTRINGHASH("champion"):
		case ATSTRINGHASH("greenwood"):
		case ATSTRINGHASH("omnisegt"):
		case ATSTRINGHASH("virtue"):
		case ATSTRINGHASH("r300"):
			return true;
		}
		return false;
	}

	bool AbstractModel::usesAttachmentCameras() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("terbyte"):
		case ATSTRINGHASH("avenger"):
		case ATSTRINGHASH("avenger2"):
		case ATSTRINGHASH("avenger3"):
		case ATSTRINGHASH("avenger4"):
		case ATSTRINGHASH("kosatka"):
			return true;
		}
		return false;
	}

	bool AbstractModel::canBeStoodUpon() const noexcept
	{
		if (auto mi = getVehicleModelInfo())
		{
			return mi->flags.get(CVehicleModelInfo::FLAG_PEDS_CAN_STAND_ON_TOP);
		}
		return false;
	}

	bool AbstractModel::isCageObject() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("prop_gold_cont_01"):
		case ATSTRINGHASH("prop_rub_cage01a"):
		case ATSTRINGHASH("prop_fnclink_03e"):
		case ATSTRINGHASH("stt_prop_stunt_tube_s"):
		case ATSTRINGHASH("mule"):
		case ATSTRINGHASH("boxville"):
		case ATSTRINGHASH("boxville2"):
		case ATSTRINGHASH("boxville3"):
		case ATSTRINGHASH("boxville4"):
		case ATSTRINGHASH("boxville5"):
		case ATSTRINGHASH("prop_gascage01"):
		case ATSTRINGHASH("hei_prop_heist_apecrate"):
		case ATSTRINGHASH("prop_container_05a"):
		case ATSTRINGHASH("prop_container_ld_pu"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isScubaObject() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("p_d_scuba_mask_s"):
		case ATSTRINGHASH("p_d_scuba_tank_s"):
		case ATSTRINGHASH("p_michael_scuba_mask_s"):
		case ATSTRINGHASH("p_michael_scuba_tank_s"):
		case ATSTRINGHASH("p_s_scuba_mask_s"):
		case ATSTRINGHASH("p_s_scuba_tank_s"):
		case ATSTRINGHASH("p_steve_scuba_hood_s"):
		case ATSTRINGHASH("xm_prop_x17_scuba_tank"):
		case ATSTRINGHASH("xm_int_lev_scuba_gear"):
		case ATSTRINGHASH("h4_int_sub_scuba_gear_002"):
		case ATSTRINGHASH("h4_int_sub_scuba_gear_01"):
		case ATSTRINGHASH("h4_int_lev_scuba_gear"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isStoryCharacter() const noexcept
	{
		return model == ATSTRINGHASH("player_zero") || model == ATSTRINGHASH("player_one") || model == ATSTRINGHASH("player_two");
	}

	bool AbstractModel::isOnlineCharacter() const noexcept
	{
		return model == ATSTRINGHASH("mp_m_freemode_01") || model == ATSTRINGHASH("mp_f_freemode_01");
	}

	bool AbstractModel::isPlayableOnlineCharacter() const noexcept
	{
		return isOnlineCharacter() || model == ATSTRINGHASH("p_franklin_02") || model == ATSTRINGHASH("ig_lamardavis_02");
	}

	bool AbstractModel::isAnimal() const noexcept
	{
		const auto ped_model = PedModel::fromHash(model);
		if (ped_model != nullptr)
		{
			switch (ped_model->category)
			{
			case ATSTRINGHASH("GRNDANMLS"):
			case ATSTRINGHASH("WTRANMLS"):
			case ATSTRINGHASH("FLYNANMLS"):
				return true;
			}
		}
		return false;
	}

	bool AbstractModel::isWaterAnimal() const noexcept
	{
		const auto ped_model = PedModel::fromHash(model);
		if (ped_model != nullptr
			&& ped_model->category == ATSTRINGHASH("WTRANMLS")
			)
		{
			return true;
		}
		return false;
	}

	bool AbstractModel::isObtainableFromPeyote() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("a_c_boar"):
		case ATSTRINGHASH("a_c_cat_01"):
		case ATSTRINGHASH("a_c_cow"):
		case ATSTRINGHASH("a_c_coyote"):
		case ATSTRINGHASH("a_c_deer"):
		case ATSTRINGHASH("a_c_husky"):
		case ATSTRINGHASH("a_c_mtlion"):
		case ATSTRINGHASH("a_c_pig"):
		case ATSTRINGHASH("a_c_poodle"):
		case ATSTRINGHASH("a_c_pug"):
		case ATSTRINGHASH("a_c_rabbit_01"):
		case ATSTRINGHASH("a_c_retriever"):
		case ATSTRINGHASH("a_c_rottweiler"):
		case ATSTRINGHASH("a_c_shepherd"):
		case ATSTRINGHASH("a_c_westy"):
		case ATSTRINGHASH("a_c_chickenhawk"):
		case ATSTRINGHASH("a_c_cormorant"):
		case ATSTRINGHASH("a_c_crow"):
		case ATSTRINGHASH("a_c_hen"):
		case ATSTRINGHASH("a_c_pigeon"):
		case ATSTRINGHASH("a_c_seagull"):
		case ATSTRINGHASH("a_c_dolphin"):
		case ATSTRINGHASH("a_c_fish"):
		case ATSTRINGHASH("a_c_killerwhale"):
		case ATSTRINGHASH("a_c_sharkhammer"):
		case ATSTRINGHASH("a_c_sharktiger"):
		case ATSTRINGHASH("a_c_stingray"):
		case ATSTRINGHASH("ig_orleans"):
		case ATSTRINGHASH("a_c_chop"):
		case ATSTRINGHASH("a_c_humpback"):
			return true;
		}
		return false;
	}

	bool AbstractModel::canBecomeHostile() const noexcept
	{
		if (isAnimal())
		{
			switch (model)
			{
			case ATSTRINGHASH("a_c_chop"):
			case ATSTRINGHASH("a_c_chop_02"):
			case ATSTRINGHASH("a_c_shepherd"):
			case ATSTRINGHASH("a_c_rottweiler"):
				return true;
			}
			return false;
		}
		return true;
	}

	bool AbstractModel::canUseProjectiles() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("a_c_rat"):
		case ATSTRINGHASH("a_c_rabbit_01"):
			return false;
		}
		return !isWaterAnimal();
	}

	bool AbstractModel::canUseWeapons() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("a_c_chimp_02"): // Used in last dose missions. Seems to not cause any issues when using guns.
			return true;
		}
		return !isAnimal();
	}

	bool AbstractModel::isCsPed() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("cs_amandatownley"):
		case ATSTRINGHASH("cs_andreas"):
		case ATSTRINGHASH("cs_ashley"):
		case ATSTRINGHASH("cs_bankman"):
		case ATSTRINGHASH("cs_barry"):
		case ATSTRINGHASH("cs_beverly"):
		case ATSTRINGHASH("cs_brad"):
		case ATSTRINGHASH("cs_bradcadaver"):
		case ATSTRINGHASH("cs_carbuyer"):
		case ATSTRINGHASH("cs_casey"):
		case ATSTRINGHASH("cs_chengsr"):
		case ATSTRINGHASH("cs_chrisformage"):
		case ATSTRINGHASH("cs_clay"):
		case ATSTRINGHASH("cs_dale"):
		case ATSTRINGHASH("cs_davenorton"):
		case ATSTRINGHASH("cs_debra"):
		case ATSTRINGHASH("cs_denise"):
		case ATSTRINGHASH("cs_devin"):
		case ATSTRINGHASH("cs_dom"):
		case ATSTRINGHASH("cs_dreyfuss"):
		case ATSTRINGHASH("cs_drfriedlander"):
		case ATSTRINGHASH("cs_drfriedlander_02"):
		case ATSTRINGHASH("cs_fabien"):
		case ATSTRINGHASH("cs_fbisuit_01"):
		case ATSTRINGHASH("cs_floyd"):
		case ATSTRINGHASH("cs_guadalope"):
		case ATSTRINGHASH("cs_gurk"): // no ig_ counterpart
		case ATSTRINGHASH("cs_hunter"):
		case ATSTRINGHASH("cs_janet"):
		case ATSTRINGHASH("cs_jewelass"):
		case ATSTRINGHASH("cs_jimmyboston"):
		case ATSTRINGHASH("cs_jimmydisanto"):
		case ATSTRINGHASH("cs_jimmydisanto2"):
		case ATSTRINGHASH("cs_joeminuteman"):
		case ATSTRINGHASH("cs_johnnyklebitz"):
		case ATSTRINGHASH("cs_josef"):
		case ATSTRINGHASH("cs_josh"):
		case ATSTRINGHASH("cs_karen_daniels"):
		case ATSTRINGHASH("cs_lamardavis"):
		case ATSTRINGHASH("cs_lamardavis_02"):
		case ATSTRINGHASH("cs_lazlow"):
		case ATSTRINGHASH("cs_lazlow_2"):
		case ATSTRINGHASH("cs_lestercrest"):
		case ATSTRINGHASH("cs_lestercrest_2"):
		case ATSTRINGHASH("cs_lestercrest_3"):
		case ATSTRINGHASH("cs_lifeinvad_01"):
		case ATSTRINGHASH("cs_magenta"):
		case ATSTRINGHASH("cs_manuel"):
		case ATSTRINGHASH("cs_marnie"):
		case ATSTRINGHASH("cs_martinmadrazo"): // no ig_ counterpart
		case ATSTRINGHASH("cs_maryann"):
		case ATSTRINGHASH("cs_michelle"):
		case ATSTRINGHASH("cs_milton"):
		case ATSTRINGHASH("cs_molly"):
		case ATSTRINGHASH("cs_movpremf_01"):
		case ATSTRINGHASH("cs_movpremmale"):
		case ATSTRINGHASH("cs_mrk"):
		case ATSTRINGHASH("cs_mrs_thornhill"):
		case ATSTRINGHASH("cs_mrsphillips"):
		case ATSTRINGHASH("cs_natalia"):
		case ATSTRINGHASH("cs_nervousron"):
		case ATSTRINGHASH("cs_nervousron_02"):
		case ATSTRINGHASH("cs_nigel"):
		case ATSTRINGHASH("cs_old_man1a"):
		case ATSTRINGHASH("cs_old_man2"):
		case ATSTRINGHASH("cs_omega"):
		case ATSTRINGHASH("cs_orleans"):
		case ATSTRINGHASH("cs_paper"):
		case ATSTRINGHASH("cs_patricia"):
		case ATSTRINGHASH("cs_patricia_02"):
		case ATSTRINGHASH("cs_priest"):
		case ATSTRINGHASH("cs_prolsec_02"):
		case ATSTRINGHASH("cs_russiandrunk"):
		case ATSTRINGHASH("cs_siemonyetarian"):
		case ATSTRINGHASH("cs_solomon"):
		case ATSTRINGHASH("cs_stevehains"):
		case ATSTRINGHASH("cs_stretch"):
		case ATSTRINGHASH("cs_tanisha"):
		case ATSTRINGHASH("cs_taocheng"):
		case ATSTRINGHASH("cs_taocheng2"):
		case ATSTRINGHASH("cs_taostranslator"):
		case ATSTRINGHASH("cs_taostranslator2"):
		case ATSTRINGHASH("cs_tenniscoach"):
		case ATSTRINGHASH("cs_terry"):
		case ATSTRINGHASH("cs_tom"):
		case ATSTRINGHASH("cs_tomepsilon"):
		case ATSTRINGHASH("cs_tracydisanto"):
		case ATSTRINGHASH("cs_wade"):
		case ATSTRINGHASH("cs_zimbor"):
		case ATSTRINGHASH("csb_abigail"):
		case ATSTRINGHASH("csb_agatha"):
		case ATSTRINGHASH("csb_agent"):
		case ATSTRINGHASH("csb_alan"):
		case ATSTRINGHASH("csb_anita"):
		case ATSTRINGHASH("csb_anton"):
		case ATSTRINGHASH("csb_ary"):
		case ATSTRINGHASH("csb_ary_02"):
		case ATSTRINGHASH("csb_avery"):
		case ATSTRINGHASH("csb_avischwartzman_02"):
		case ATSTRINGHASH("csb_avischwartzman_03"):
		case ATSTRINGHASH("csb_avon"):
		case ATSTRINGHASH("csb_ballas_leader"):
		case ATSTRINGHASH("csb_ballasog"):
		case ATSTRINGHASH("csb_billionaire"):
		case ATSTRINGHASH("csb_bogdan"):
		case ATSTRINGHASH("csb_bride"):
		case ATSTRINGHASH("csb_brucie2"):
		case ATSTRINGHASH("csb_bryony"):
		case ATSTRINGHASH("csb_burgerdrug"):
		case ATSTRINGHASH("csb_car3guy1"):
		case ATSTRINGHASH("csb_car3guy2"):
		case ATSTRINGHASH("csb_celeb_01"):
		case ATSTRINGHASH("csb_charlie_reed"):
		case ATSTRINGHASH("csb_chef"):
		case ATSTRINGHASH("csb_chef2"):
		case ATSTRINGHASH("CSB_Chef_03"):
		case ATSTRINGHASH("csb_chin_goon"):
		case ATSTRINGHASH("csb_cletus"):
		case ATSTRINGHASH("csb_cop"):
		case ATSTRINGHASH("csb_customer"):
		case ATSTRINGHASH("CSB_Dax"):
		case ATSTRINGHASH("csb_denise_friend"):
		case ATSTRINGHASH("csb_dix"):
		case ATSTRINGHASH("csb_djblamadon"):
		case ATSTRINGHASH("csb_drugdealer"):
		case ATSTRINGHASH("csb_englishdave"):
		case ATSTRINGHASH("csb_englishdave_02"):
		case ATSTRINGHASH("csb_fos_rep"):
		case ATSTRINGHASH("csb_g"):
		case ATSTRINGHASH("csb_georginacheng"):
		case ATSTRINGHASH("csb_golfer_a"):
		case ATSTRINGHASH("csb_golfer_b"):
		case ATSTRINGHASH("csb_groom"):
		case ATSTRINGHASH("csb_grove_str_dlr"):
		case ATSTRINGHASH("csb_gustavo"):
		case ATSTRINGHASH("csb_hao"):
		case ATSTRINGHASH("csb_hao_02"):
		case ATSTRINGHASH("csb_helmsmanpavel"):
		case ATSTRINGHASH("csb_huang"):
		case ATSTRINGHASH("csb_hugh"):
		case ATSTRINGHASH("csb_imani"):
		case ATSTRINGHASH("csb_imran"):
		case ATSTRINGHASH("csb_isldj_00"):
		case ATSTRINGHASH("csb_isldj_01"):
		case ATSTRINGHASH("csb_isldj_02"):
		case ATSTRINGHASH("csb_isldj_03"):
		case ATSTRINGHASH("csb_isldj_04"):
		case ATSTRINGHASH("csb_jackhowitzer"):
		case ATSTRINGHASH("csb_janitor"):
		case ATSTRINGHASH("csb_jio"):
		case ATSTRINGHASH("csb_jio_02"):
		case ATSTRINGHASH("csb_johnny_guns"):
		case ATSTRINGHASH("csb_juanstrickler"):
		case ATSTRINGHASH("CSB_Labrat"):
		case ATSTRINGHASH("CSB_Luchadora"):
		case ATSTRINGHASH("csb_maude"):
		case ATSTRINGHASH("csb_miguelmadrazo"):
		case ATSTRINGHASH("csb_mimi"):
		case ATSTRINGHASH("csb_mjo"):
		case ATSTRINGHASH("csb_mjo_02"):
		case ATSTRINGHASH("csb_money"):
		case ATSTRINGHASH("csb_moodyman_02"):
		case ATSTRINGHASH("csb_mp_agent14"):
		case ATSTRINGHASH("csb_mrs_r"):
		case ATSTRINGHASH("csb_musician_00"):
		case ATSTRINGHASH("csb_mweather"):
		case ATSTRINGHASH("csb_ortega"):
		case ATSTRINGHASH("csb_oscar"):
		case ATSTRINGHASH("csb_paige"):
		case ATSTRINGHASH("csb_party_promo"):
		case ATSTRINGHASH("csb_popov"):
		case ATSTRINGHASH("csb_porndudes"):
		case ATSTRINGHASH("csb_prologuedriver"):
		case ATSTRINGHASH("csb_prolsec"):
		case ATSTRINGHASH("csb_ramp_gang"):
		case ATSTRINGHASH("csb_ramp_hic"):
		case ATSTRINGHASH("csb_ramp_hipster"):
		case ATSTRINGHASH("csb_ramp_marine"):
		case ATSTRINGHASH("csb_ramp_mex"):
		case ATSTRINGHASH("csb_rashcosvki"):
		case ATSTRINGHASH("csb_reporter"):
		case ATSTRINGHASH("csb_req_officer"):
		case ATSTRINGHASH("csb_roccopelosi"):
		case ATSTRINGHASH("csb_screen_writer"):
		case ATSTRINGHASH("csb_security_a"):
		case ATSTRINGHASH("csb_sessanta"):
		case ATSTRINGHASH("csb_sol"):
		case ATSTRINGHASH("csb_soundeng_00"):
		case ATSTRINGHASH("csb_sss"):
		case ATSTRINGHASH("csb_stripper_01"):
		case ATSTRINGHASH("csb_stripper_02"):
		case ATSTRINGHASH("csb_talcc"):
		case ATSTRINGHASH("csb_talmm"):
		case ATSTRINGHASH("csb_thornton"):
		case ATSTRINGHASH("csb_tomcasino"):
		case ATSTRINGHASH("csb_tonya"):
		case ATSTRINGHASH("csb_tonyprince"):
		case ATSTRINGHASH("csb_trafficwarden"):
		case ATSTRINGHASH("csb_undercover"):
		case ATSTRINGHASH("csb_vagos_leader"):
		case ATSTRINGHASH("csb_vagspeak"):
		case ATSTRINGHASH("csb_vernon"):
		case ATSTRINGHASH("csb_vincent"):
		case ATSTRINGHASH("csb_vincent_2"):
		case ATSTRINGHASH("csb_wendy"):
			return true;
		}
		return false;
	}

	bool AbstractModel::isHiResPed() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("player_zero"):
		case ATSTRINGHASH("player_one"):
		case ATSTRINGHASH("player_two"):
		case ATSTRINGHASH("p_franklin_02"):
		case ATSTRINGHASH("ig_amandatownley"):
		case ATSTRINGHASH("ig_brad"):
		case ATSTRINGHASH("ig_josh"):
		case ATSTRINGHASH("ig_lamardavis_02"):
		case ATSTRINGHASH("ig_lazlow"):
		case ATSTRINGHASH("ig_lazlow_2"):
		case ATSTRINGHASH("ig_lestercrest"):
		case ATSTRINGHASH("ig_nervousron"):
		case ATSTRINGHASH("ig_tracydisanto"):
		case ATSTRINGHASH("ig_wade"):
			return true;
		}
		return isCsPed();
	}

	bool AbstractModel::isSusAttachee() const noexcept
	{
		// Lots of objects are attached to the cargoplane in "last dose 5" apparently
		if (model == ATSTRINGHASH("cargoplane2"))
		{
			return false;
		}

		switch (model)
		{
		case ATSTRINGHASH("barracks"):
		case ATSTRINGHASH("barracks3"):
		case ATSTRINGHASH("chernobog"):
		case ATSTRINGHASH("dinghy3"):
		case ATSTRINGHASH("dune"):
		case ATSTRINGHASH("scrap"):
			return true;
		}
		return isBigVehicle() || isPlane();
	}

	bool AbstractModel::isSusAttachment() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("cargobob"):
			return true;
		}
		return isSusAttachee();
	}

	bool AbstractModel::isValidParachute() const noexcept
	{
		switch (model)
		{
			// Ped
		case ATSTRINGHASH("lts_p_para_pilot2_sp_s"):
		case ATSTRINGHASH("p_parachute1_mp_dec"):
		case ATSTRINGHASH("p_parachute1_mp_s"):
		case ATSTRINGHASH("p_parachute1_s"):
		case ATSTRINGHASH("p_parachute1_sp_dec"):
		case ATSTRINGHASH("p_parachute1_sp_s"):
		case ATSTRINGHASH("pil_p_para_pilot_sp_s"):
		case ATSTRINGHASH("prop_v_parachute"):
			// Vehicle
		case ATSTRINGHASH("imp_prop_impexp_para_s"):
		case ATSTRINGHASH("sr_prop_specraces_para_s_01"):
		case ATSTRINGHASH("gr_prop_gr_para_s_01"):
			// Not valid per-se, but not causing a crash afaik
		case ATSTRINGHASH("p_para_broken1_s"):
			// Added with b2699
		case ATSTRINGHASH("reh_prop_reh_para_sp_s_01a"):
			// Added in tuners
		case ATSTRINGHASH("tr_prop_tr_para_sp_s_01a"):
			return true;
		}
		return false;
	}

	bool AbstractModel::supportsInteriorColours() const noexcept
	{
		switch (model)
		{
		case ATSTRINGHASH("ardent"):
		case ATSTRINGHASH("asbo"):
		case ATSTRINGHASH("avisa"):
		case ATSTRINGHASH("baller3"):
		case ATSTRINGHASH("baller4"):
		case ATSTRINGHASH("baller5"):
		case ATSTRINGHASH("baller6"):
		case ATSTRINGHASH("baller7"):
		case ATSTRINGHASH("baller8"):
		case ATSTRINGHASH("brigham"):
		case ATSTRINGHASH("brioso3"):
		case ATSTRINGHASH("buccaneer2"):
		case ATSTRINGHASH("buffalo4"):
		case ATSTRINGHASH("buffalo5"):
		case ATSTRINGHASH("castigator"):
		case ATSTRINGHASH("champion"):
		case ATSTRINGHASH("cheburek"):
		case ATSTRINGHASH("cheetah2"):
		case ATSTRINGHASH("chino2"):
		case ATSTRINGHASH("clique"):
		case ATSTRINGHASH("clique2"):
		case ATSTRINGHASH("cog55"):
		case ATSTRINGHASH("cog552"):
		case ATSTRINGHASH("cognoscenti"):
		case ATSTRINGHASH("cognoscenti2"):
		case ATSTRINGHASH("comet6"):
		case ATSTRINGHASH("comet7"):
		case ATSTRINGHASH("coquette4"):
		case ATSTRINGHASH("coquette5"):
		case ATSTRINGHASH("corsita"):
		case ATSTRINGHASH("coureur"):
		case ATSTRINGHASH("cyclone"):
		case ATSTRINGHASH("cypher"):
		case ATSTRINGHASH("deity"):
		case ATSTRINGHASH("deluxo"):
		case ATSTRINGHASH("deveste"):
		case ATSTRINGHASH("dominator10"):
		case ATSTRINGHASH("dominator8"):
		case ATSTRINGHASH("dominator9"):
		case ATSTRINGHASH("driftcypher"):
		case ATSTRINGHASH("driftjester"):
		case ATSTRINGHASH("driftsentinel"):
		case ATSTRINGHASH("driftyosemite"):
		case ATSTRINGHASH("driftzr350"):
		case ATSTRINGHASH("ellie"):
		case ATSTRINGHASH("entity2"):
		case ATSTRINGHASH("entity3"):
		case ATSTRINGHASH("envisage"):
		case ATSTRINGHASH("eudora"):
		case ATSTRINGHASH("faction2"):
		case ATSTRINGHASH("faction3"):
		case ATSTRINGHASH("fagaloa"):
		case ATSTRINGHASH("gauntlet3"):
		case ATSTRINGHASH("gauntlet4"):
		case ATSTRINGHASH("gauntlet5"):
		case ATSTRINGHASH("gb200"):
		case ATSTRINGHASH("glendale2"):
		case ATSTRINGHASH("gp1"):
		case ATSTRINGHASH("growler"):
		case ATSTRINGHASH("ignus"):
		case ATSTRINGHASH("imorgon"):
		case ATSTRINGHASH("impaler"):
		case ATSTRINGHASH("impaler6"):
		case ATSTRINGHASH("infernus2"):
		case ATSTRINGHASH("issi3"):
		case ATSTRINGHASH("issi7"):
		case ATSTRINGHASH("italigto"):
		case ATSTRINGHASH("italirsx"):
		case ATSTRINGHASH("jester4"):
		case ATSTRINGHASH("jugular"):
		case ATSTRINGHASH("krieger"):
		case ATSTRINGHASH("l35"):
		case ATSTRINGHASH("le7b"):
		case ATSTRINGHASH("lm87"):
		case ATSTRINGHASH("manana2"):
		case ATSTRINGHASH("minivan2"):
		case ATSTRINGHASH("moonbeam2"):
		case ATSTRINGHASH("neo"):
		case ATSTRINGHASH("neon"):
		case ATSTRINGHASH("nero"):
		case ATSTRINGHASH("nero2"):
		case ATSTRINGHASH("niobe"):
		case ATSTRINGHASH("omnis"):
		case ATSTRINGHASH("paragon"):
		case ATSTRINGHASH("paragon2"):
		case ATSTRINGHASH("paragon3"):
		case ATSTRINGHASH("patriot3"):
		case ATSTRINGHASH("peyote3"):
		case ATSTRINGHASH("pipistrello"):
		case ATSTRINGHASH("poldominator10"):
		case ATSTRINGHASH("polgauntlet"):
		case ATSTRINGHASH("polimpaler6"):
		case ATSTRINGHASH("postlude"):
		case ATSTRINGHASH("primo2"):
		case ATSTRINGHASH("r300"):
		case ATSTRINGHASH("rapidgt3"):
		case ATSTRINGHASH("revolter"):
		case ATSTRINGHASH("rhinehart"):
		case ATSTRINGHASH("rt3000"):
		case ATSTRINGHASH("ruston"):
		case ATSTRINGHASH("sabregt2"):
		case ATSTRINGHASH("sc1"):
		case ATSTRINGHASH("schafter3"):
		case ATSTRINGHASH("schafter4"):
		case ATSTRINGHASH("schafter5"):
		case ATSTRINGHASH("schafter6"):
		case ATSTRINGHASH("schlagen"):
		case ATSTRINGHASH("sentinel3"):
		case ATSTRINGHASH("sentinel4"):
		case ATSTRINGHASH("slamvan3"):
		case ATSTRINGHASH("sm722"):
		case ATSTRINGHASH("specter"):
		case ATSTRINGHASH("specter2"):
		case ATSTRINGHASH("stafford"):
		case ATSTRINGHASH("stingertt"):
		case ATSTRINGHASH("stromberg"):
		case ATSTRINGHASH("sugoi"):
		case ATSTRINGHASH("swinger"):
		case ATSTRINGHASH("tahoma"):
		case ATSTRINGHASH("taipan"):
		case ATSTRINGHASH("tenf"):
		case ATSTRINGHASH("tenf2"):
		case ATSTRINGHASH("terminus"):
		case ATSTRINGHASH("tezeract"):
		case ATSTRINGHASH("thrax"):
		case ATSTRINGHASH("toreador"):
		case ATSTRINGHASH("torero"):
		case ATSTRINGHASH("torero2"):
		case ATSTRINGHASH("tornado5"):
		case ATSTRINGHASH("toros"):
		case ATSTRINGHASH("trophytruck"):
		case ATSTRINGHASH("trophytruck2"):
		case ATSTRINGHASH("tulip2"):
		case ATSTRINGHASH("turismo2"):
		case ATSTRINGHASH("turismo3"):
		case ATSTRINGHASH("tyrus"):
		case ATSTRINGHASH("vagner"):
		case ATSTRINGHASH("vectre"):
		case ATSTRINGHASH("vigero2"):
		case ATSTRINGHASH("vigero3"):
		case ATSTRINGHASH("virgo2"):
		case ATSTRINGHASH("virtue"):
		case ATSTRINGHASH("viseris"):
		case ATSTRINGHASH("visione"):
		case ATSTRINGHASH("voodoo"):
		case ATSTRINGHASH("weevil"):
		case ATSTRINGHASH("weevil2"):
		case ATSTRINGHASH("windsor2"):
		case ATSTRINGHASH("xa21"):
		case ATSTRINGHASH("yosemite"):
		case ATSTRINGHASH("yosemite1500"):
		case ATSTRINGHASH("yosemite2"):
		case ATSTRINGHASH("yosemite3"):
		case ATSTRINGHASH("youga3"):
		case ATSTRINGHASH("z190"):
		case ATSTRINGHASH("zeno"):
		case ATSTRINGHASH("zion3"):
		case ATSTRINGHASH("zr350"):
			return true;
		}

		return false;
	}
}
