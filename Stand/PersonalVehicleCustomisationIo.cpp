#include "PersonalVehicleCustomisationIo.hpp"

#include <utility>

#include "natives.hpp"
#include "StatUtil.hpp"
#include "VehicleColour.hpp"

namespace Stand
{
	Mixed PersonalVehicleCustomisationIo::read(int idx)
	{
		auto base = pv.getBase();
		if (isVehicleMod(idx))
		{
			idx = translatePropertyToVehicleMod(idx);
			int val = base.at(GLOBAL_PVS_MODS_ARR).at(idx, 1).get<int>();
			if (VehicleMods::isBooleanExcludeLights(idx))
			{
				return bool(val > 0);
			}
			if (idx == VehicleMods::front_wheels)
			{
				return std::pair<int, bool>(val, base.at(GLOBAL_PVS_MODS_VAR_ARR).at(GLOBAL_PVS_MODS_VAR_CUSTOM_FRONT_TYRES, 1).get<int>() > 0);
			}
			if (idx == VehicleMods::back_wheels)
			{
				return std::pair<int, bool>(val, base.at(GLOBAL_PVS_MODS_VAR_ARR).at(GLOBAL_PVS_MODS_VAR_CUSTOM_BACK_TYRES, 1).get<int>() > 0);
			}
			return val;
		}
		switch (idx)
		{
		case MODEL:
			return base.at(GLOBAL_PVS_MODEL).get<Hash>();

		case PLATE_STYLE:
			return base.at(GLOBAL_PVS_PLATE_STYLE).get<int>();

		case PLATE_TEXT:
			return base.at(GLOBAL_PVS_PLATE_TEXT).as<const char*>();

		case PRIMARY_COLOUR:
			return base.at(GLOBAL_PVS_PRIMARY_COLOUR).get<int>();

		case CUSTOM_PRIMARY_COLOUR:
		{
			OptVehicleColour c{};
			if (pv.getFlag(GLOBAL_PVS_FLAGS_CUSTOM_PRIMARY_COLOUR))
			{
				c.is_applicable = true;
				c.colour.r = base.at(GLOBAL_PVS_CUSTOM_R).get<int>();
				c.colour.g = base.at(GLOBAL_PVS_CUSTOM_G).get<int>();
				c.colour.b = base.at(GLOBAL_PVS_CUSTOM_B).get<int>();
			}
			return c;
		}

		case SECONDARY_COLOUR:
			return base.at(GLOBAL_PVS_SECONDARY_COLOUR).get<int>();

		case CUSTOM_SECONDARY_COLOUR:
		{
			OptVehicleColour c{};
			if (pv.getFlag(GLOBAL_PVS_FLAGS_CUSTOM_SECONDARY_COLOUR))
			{
				c.is_applicable = true;
				c.colour.r = base.at(GLOBAL_PVS_CUSTOM_R).get<int>();
				c.colour.g = base.at(GLOBAL_PVS_CUSTOM_G).get<int>();
				c.colour.b = base.at(GLOBAL_PVS_CUSTOM_B).get<int>();
			}
			return c;
		}
			
		case PEARL_COLOUR:
			return base.at(GLOBAL_PVS_PEARL_COLOUR).get<int>();

		case WHEEL_COLOUR:
			return base.at(GLOBAL_PVS_WHEEL_COLOUR).get<int>();

		case ENVEFF_SCALE:
			return base.at(GLOBAL_PVS_ENVEFF_SCALE).get<float>();

		case INTERIOR_COLOUR:
			return base.at(GLOBAL_PVS_INTERIOR_COLOUR).get<int>();

		case ROOF_LIVERY:
			return base.at(GLOBAL_PVS_ROOF_LIVERY).get<int>();
		
		case DASHBOARD_COLOUR:
			return base.at(GLOBAL_PVS_DASHBOARD_COLOUR).get<int>();

		case TYRES_MODE:
			return base.at(GLOBAL_PVS_TYRES_MODE).get<int>();

		case TYRE_SMOKE_COLOUR:
		{
			VehicleColour c;
			c.r = base.at(GLOBAL_PVS_TYRE_SMOKE_R).get<int>();
			c.g = base.at(GLOBAL_PVS_TYRE_SMOKE_G).get<int>();
			c.b = base.at(GLOBAL_PVS_TYRE_SMOKE_B).get<int>();
			return c;
		}

		case WINDOW_TINT:
			return base.at(GLOBAL_PVS_WINDOW_TINT).get<int>();
		
		case NEON_COLOUR:
		{
			VehicleColour c;
			c.r = base.at(GLOBAL_PVS_NEON_R).get<int>();
			c.g = base.at(GLOBAL_PVS_NEON_G).get<int>();
			c.b = base.at(GLOBAL_PVS_NEON_B).get<int>();
			return c;
		}

		case NEON_FRONT:
			return pv.getFlag(GLOBAL_PVS_FLAGS_NEON_FRONT);

		case NEON_BACK:
			return pv.getFlag(GLOBAL_PVS_FLAGS_NEON_BACK);

		case NEON_LEFT:
			return pv.getFlag(GLOBAL_PVS_FLAGS_NEON_LEFT);

		case NEON_RIGHT:
			return pv.getFlag(GLOBAL_PVS_FLAGS_NEON_RIGHT);

		case WHEEL_TYPE:
			return base.at(GLOBAL_PVS_WHEEL_TYPE).get<int>();

		case EXTRA_1:
		case EXTRA_2:
		case EXTRA_3:
		case EXTRA_4:
		case EXTRA_5:
		case EXTRA_6:
		case EXTRA_7:
		case EXTRA_8:
		case EXTRA_9:
		case EXTRA_10:
		case EXTRA_11:
		case EXTRA_12:
			return pv.getFlag(extraToFlagsIndex((idx - EXTRA_1) + 1));
		case EXTRA_13:
		case EXTRA_14:
		case EXTRA_15:
			return true;
		}
		return -1;
	}

	void PersonalVehicleCustomisationIo::write(int idx, Mixed&& value)
	{
#if false
		auto base = pv.getBase();
		if (isVehicleMod(idx))
		{
			idx = translatePropertyToVehicleMod(idx);
			if (VehicleMods::isBooleanExcludeLights(idx))
			{
				base.at(GLOBAL_PVS_MODS_ARR).at(idx, 1).set<int>(value.get<bool>() ? 1 : 0);
			}
			else if (idx == VehicleMods::front_wheels)
			{
				auto val = value.as<std::pair<int, bool>>();
				base.at(GLOBAL_PVS_MODS_ARR).at(23, 1).set<int>(val->first);
				base.at(GLOBAL_PVS_MODS_VAR_ARR).at(GLOBAL_PVS_MODS_VAR_CUSTOM_FRONT_TYRES, 1).set<int>(val->second ? 1 : 0);
				StatUtil::setPackedInt(getPackedIntIndex(1439) + GLOBAL_PVS_MODS_VAR_CUSTOM_FRONT_TYRES, val->second ? 1 : 0, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			}
			else if (idx == VehicleMods::back_wheels)
			{
				auto val = value.as<std::pair<int, bool>>();
				base.at(GLOBAL_PVS_MODS_ARR).at(24, 1).set<int>(val->first);
				base.at(GLOBAL_PVS_MODS_VAR_ARR).at(GLOBAL_PVS_MODS_VAR_CUSTOM_BACK_TYRES, 1).set<int>(val->second ? 1 : 0);
				StatUtil::setPackedInt(getPackedIntIndex(1439) + GLOBAL_PVS_MODS_VAR_CUSTOM_BACK_TYRES, val->second ? 1 : 0, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			}
			else
			{
				base.at(GLOBAL_PVS_MODS_ARR).at(idx, 1).set<int>(value);
			}
			if (idx < 25)
			{
				StatUtil::setPackedInt(getPackedIntIndex(1414) + idx, base.at(GLOBAL_PVS_MODS_ARR).at(idx, 1).get<int>(), ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			}
			else
			{
				StatUtil::setPackedInt(getPackedIntHighModTypeIndex() + (idx - 25), base.at(GLOBAL_PVS_MODS_ARR).at(idx, 1).get<int>(), ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			}
		}
		else switch (idx)
		{
		case MODEL:
			//script_global(2359296).at(0, 5559).at(675).at(1275).set<int>(NETWORK::GET_CLOUD_TIME_AS_INT());

			STATS::STAT_SET_INT(CHAR_STAT(ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>(), "mpsv_vehicle_timestamp"), NETWORK::GET_CLOUD_TIME_AS_INT(), TRUE);

			base.at(GLOBAL_PVS_MODEL).set<Hash>(value);
			STATS::STAT_SET_INT(pv.getStatHash("model"), value, true);
			break;

		case PLATE_STYLE:
			base.at(GLOBAL_PVS_PLATE_STYLE).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1413), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case PLATE_TEXT:
			strcpy(base.at(GLOBAL_PVS_PLATE_TEXT).as<char*>(), value);
			STATS::STAT_SET_LICENSE_PLATE(pv.getStatHash("lp0"), value);
			break;

		case PRIMARY_COLOUR:
			base.at(GLOBAL_PVS_PRIMARY_COLOUR).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1452), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case CUSTOM_PRIMARY_COLOUR:
		{
			auto c = value.as<OptVehicleColour>();
			pv.setFlag(GLOBAL_PVS_FLAGS_CUSTOM_PRIMARY_COLOUR, c->is_applicable);
			if (c->is_applicable)
			{
				base.at(GLOBAL_PVS_CUSTOM_R).set<int>(c->colour.r);
				base.at(GLOBAL_PVS_CUSTOM_G).set<int>(c->colour.g);
				base.at(GLOBAL_PVS_CUSTOM_B).set<int>(c->colour.b);
				writeCustomColourStats(base);
			}
			break;
		}

		case SECONDARY_COLOUR:
			base.at(GLOBAL_PVS_SECONDARY_COLOUR).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1453), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case CUSTOM_SECONDARY_COLOUR:
		{
			auto c = value.as<OptVehicleColour>();
			pv.setFlag(GLOBAL_PVS_FLAGS_CUSTOM_SECONDARY_COLOUR, c->is_applicable);
			if (c->is_applicable)
			{
				base.at(GLOBAL_PVS_CUSTOM_R).set<int>(c->colour.r);
				base.at(GLOBAL_PVS_CUSTOM_G).set<int>(c->colour.g);
				base.at(GLOBAL_PVS_CUSTOM_B).set<int>(c->colour.b);
				writeCustomColourStats(base);
			}
			break;
		}

		case PEARL_COLOUR:
			base.at(GLOBAL_PVS_PEARL_COLOUR).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1454), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case WHEEL_COLOUR:
			base.at(GLOBAL_PVS_WHEEL_COLOUR).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1455), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case ENVEFF_SCALE:
			base.at(GLOBAL_PVS_ENVEFF_SCALE).set<float>(value);
			// has no stat
			break;

		case INTERIOR_COLOUR:
			base.at(GLOBAL_PVS_INTERIOR_COLOUR).set<int>(value);
			StatUtil::setPackedInt(getPackedIntInteriorColourIndex(), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case ROOF_LIVERY:
			base.at(GLOBAL_PVS_ROOF_LIVERY).set<int>(value);
			StatUtil::setPackedInt(getPackedIntRoofLiveryIndex(), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case DASHBOARD_COLOUR:
			base.at(GLOBAL_PVS_DASHBOARD_COLOUR).set<int>(value);
			StatUtil::setPackedInt(getPackedIntDashboardColourIndex(), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case TYRES_MODE:
			base.at(GLOBAL_PVS_TYRES_MODE).set<int>(value);
			StatUtil::setPackedInt(getPackedIntTyresModeIndex(), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case TYRE_SMOKE_COLOUR:
		{
			auto val = value.as<VehicleColour>();
			base.at(GLOBAL_PVS_TYRE_SMOKE_R).set<int>(val->r);
			base.at(GLOBAL_PVS_TYRE_SMOKE_G).set<int>(val->g);
			base.at(GLOBAL_PVS_TYRE_SMOKE_B).set<int>(val->b);
			StatUtil::setPackedInt(getPackedIntIndex(1441), val->r, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			StatUtil::setPackedInt(getPackedIntIndex(1442), val->g, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			StatUtil::setPackedInt(getPackedIntIndex(1443), val->b, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;
		}

		case WINDOW_TINT:
			base.at(GLOBAL_PVS_WINDOW_TINT).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1444), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case NEON_COLOUR:
		{
			auto val = value.as<VehicleColour>();
			base.at(GLOBAL_PVS_NEON_R).set<int>(val->r);
			base.at(GLOBAL_PVS_NEON_G).set<int>(val->g);
			base.at(GLOBAL_PVS_NEON_B).set<int>(val->b);
			StatUtil::setPackedInt(getPackedIntIndex(3880), val->r, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			StatUtil::setPackedInt(getPackedIntIndex(3881), val->g, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			StatUtil::setPackedInt(getPackedIntIndex(3882), val->b, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;
		}

		case NEON_FRONT:
			pv.setFlag(GLOBAL_PVS_FLAGS_NEON_FRONT, value);
			break;

		case NEON_BACK:
			pv.setFlag(GLOBAL_PVS_FLAGS_NEON_BACK, value);
			break;

		case NEON_LEFT:
			pv.setFlag(GLOBAL_PVS_FLAGS_NEON_LEFT, value);
			break;

		case NEON_RIGHT:
			pv.setFlag(GLOBAL_PVS_FLAGS_NEON_RIGHT, value);
			break;

		case WHEEL_TYPE:
			base.at(GLOBAL_PVS_WHEEL_TYPE).set<int>(value);
			StatUtil::setPackedInt(getPackedIntIndex(1447), value, ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
			break;

		case EXTRA_1:
		case EXTRA_2:
		case EXTRA_3:
		case EXTRA_4:
		case EXTRA_5:
		case EXTRA_6:
		case EXTRA_7:
		case EXTRA_8:
		case EXTRA_9:
		case EXTRA_10:
		case EXTRA_11:
		case EXTRA_12:
			pv.setFlag(extraToFlagsIndex((idx - EXTRA_1) + 1), value.get<bool>());
			break;
		case EXTRA_13:
		case EXTRA_14:
		case EXTRA_15:
			break;
		}

		if (idx == _NUM_PROPERIES - 1)
		{
			pv.saveStats();
		}
#endif
	}

	void PersonalVehicleCustomisationIo::writeCustomColourStats(ScriptGlobal base) const
	{
		StatUtil::setPackedInt(getPackedIntIndex(1449), base.at(GLOBAL_PVS_CUSTOM_R).get<int>(), ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
		StatUtil::setPackedInt(getPackedIntIndex(1450), base.at(GLOBAL_PVS_CUSTOM_G).get<int>(), ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
		StatUtil::setPackedInt(getPackedIntIndex(1451), base.at(GLOBAL_PVS_CUSTOM_B).get<int>(), ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>());
	}

	static constexpr int func_1120(const int iParam0)
	{
		switch (iParam0)
		{
		case 8:
			return 108;
			break;

		case 9:
			return 128;
			break;

		case 10:
			return 148;
			break;

		case 11:
			return 156;
			break;

		case 6:
			return 75;
			break;

		case 7:
			return 88;
			break;

		case 5:
			return -1;
			break;

		case 12:
			return 179;
			break;

		case 13:
			return 186;
			break;

		case 14:
			return 192;
			break;

		case 15:
			return 202;
			break;

		case 16:
			return 212;
			break;

		case 17:
			return 222;
			break;

		case 18:
			return 236;
			break;

		case 19:
			return 246;
			break;

		case 20:
			return 256;
			break;

		case 21:
			return 268;
			break;

		case 22:
			return 278;
			break;

		case 23:
			return 294;
			break;

		case 24:
			return 307;
			break;

		case 25:
			return 317;
			break;

		case 26:
			return 337;
			break;
		}

		return 0;
	}

	int PersonalVehicleCustomisationIo::getPackedIntIndex(const int iParam0) const
	{
		const int iParam1 = pv;

		int iVar0;

		if ((iParam0 == 3880 || iParam0 == 3881) || iParam0 == 3882)
		{
			if (iParam1 < 10)
			{
				iVar0 = (iParam0 + iParam1 * 3);
			}
			else if (iParam1 < 23)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (3910 + (iParam1 - 13) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (3911 + (iParam1 - 13) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (3912 + (iParam1 - 13) * 3);
				}
			}
			else if (iParam1 < 36)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (3946 + (iParam1 - 26) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (3947 + (iParam1 - 26) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (3948 + (iParam1 - 26) * 3);
				}
			}
			else if (iParam1 < 49)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (5855 + (iParam1 - 39) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (5856 + (iParam1 - 39) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (5857 + (iParam1 - 39) * 3);
				}
			}
			else if (iParam1 < 62)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (7149 + (iParam1 - 52) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (7150 + (iParam1 - 52) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (7151 + (iParam1 - 52) * 3);
				}
			}
			else if (iParam1 < 85)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (9107 + (iParam1 - 65) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (9108 + (iParam1 - 65) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (9109 + (iParam1 - 65) * 3);
				}
			}
			else if (iParam1 < func_1120(11))
			{
				if (iParam0 == 3880)
				{
					iVar0 = (14384 + (iParam1 - 88) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (14385 + (iParam1 - 88) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (14386 + (iParam1 - 88) * 3);
				}
			}
			else if (iParam1 <= 157)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (14384 + (iParam1 - 88) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (14385 + (iParam1 - 88) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (14386 + (iParam1 - 88) * 3);
				}
			}
			else if (iParam1 == 158)
			{
				if (iParam0 == 3880)
				{
					iVar0 = 15351;
				}
				else if (iParam0 == 3881)
				{
					iVar0 = 15352;
				}
				else if (iParam0 == 3882)
				{
					iVar0 = 15353;
				}
			}
			else if (iParam1 < 184)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (17786 + (iParam1 - 159) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (17787 + (iParam1 - 159) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (17788 + (iParam1 - 159) * 3);
				}
			}
			else if (iParam1 < 194)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (18853 + (iParam1 - 184) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (18854 + (iParam1 - 184) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (18855 + (iParam1 - 184) * 3);
				}
			}
			else if (iParam1 < 231)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (21572 + (iParam1 - 194) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (21573 + (iParam1 - 194) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (21574 + (iParam1 - 194) * 3);
				}
			}
			else if (iParam1 < 261)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (24265 + (iParam1 - 231) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (24266 + (iParam1 - 231) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (24267 + (iParam1 - 231) * 3);
				}
			}
			else if (iParam1 < 271)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (26229 + (iParam1 - 261) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (26230 + (iParam1 - 261) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (26231 + (iParam1 - 261) * 3);
				}
			}
			else if (iParam1 < 281)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (27949 + (iParam1 - 271) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (27950 + (iParam1 - 271) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (27951 + (iParam1 - 271) * 3);
				}
			}
			else if (iParam1 < 291)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (29474 + (iParam1 - 281) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (29475 + (iParam1 - 281) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (29476 + (iParam1 - 281) * 3);
				}
			}
			else if (iParam1 < 304)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (29504 + (iParam1 - 294) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (29505 + (iParam1 - 294) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (29506 + (iParam1 - 294) * 3);
				}
			}
			else if (iParam1 > 306 && iParam1 < 317)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (31158 + (iParam1 - 307) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (31159 + (iParam1 - 307) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (31160 + (iParam1 - 307) * 3);
				}
			}
			else if (iParam1 >= 317 && iParam1 < 337)
			{
				if (iParam0 == 3880)
				{
					iVar0 = (33376 + (iParam1 - 317) * 3);
				}
				else if (iParam0 == 3881)
				{
					iVar0 = (33377 + (iParam1 - 317) * 3);
				}
				else if (iParam0 == 3882)
				{
					iVar0 = (33378 + (iParam1 - 317) * 3);
				}
			}
		}
		else if (iParam1 < 10)
		{
			iVar0 = (iParam0 + (iParam1 * (1456 - 1411)));
		}
		else if (iParam1 >= 10 && iParam1 <= 12)
		{
			iVar0 = ((iParam1 - 10) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 1861;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 1862;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 1863;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 1864;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 1865;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 1866;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 1867;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 1868;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 1869;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 1870;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 1871;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 1872;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 1873;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 1874;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 1875;
			}
		}
		else if (iParam1 < 23)
		{
			iVar0 = (1906 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 13) * (1456 - 1411)));
		}
		else if (iParam1 >= 23 && iParam1 <= 25)
		{
			iVar0 = ((iParam1 - 23) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 2356;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 2357;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 2358;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 2359;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 2360;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 2361;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 2362;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 2363;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 2364;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 2365;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 2366;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 2367;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 2368;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 2369;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 2370;
			}
		}
		else if (iParam1 < 36)
		{
			iVar0 = (2405 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 26) * (1456 - 1411)));
		}
		else if (iParam1 >= 36 && iParam1 <= 38)
		{
			iVar0 = ((iParam1 - 36) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 2855;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 2856;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 2857;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 2858;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 2859;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 2860;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 2861;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 2862;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 2863;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 2864;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 2865;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 2866;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 2867;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 2868;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 2869;
			}
		}
		else if (iParam1 < 49)
		{
			iVar0 = (5360 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 39) * (1456 - 1411)));
		}
		else if (iParam1 >= 49 && iParam1 <= 51)
		{
			iVar0 = ((iParam1 - 49) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 5810;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 5811;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 5812;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 5813;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 5814;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 5815;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 5816;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 5817;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 5818;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 5819;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 5820;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 5821;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 5822;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 5823;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 5824;
			}
		}
		else if (iParam1 < 62)
		{
			iVar0 = (6654 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 52) * (1456 - 1411)));
		}
		else if (iParam1 >= 62 && iParam1 <= 64)
		{
			iVar0 = ((iParam1 - 62) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 7104;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 7105;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 7106;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 7107;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 7108;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 7109;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 7110;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 7111;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 7112;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 7113;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 7114;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 7115;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 7116;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 7117;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 7118;
			}
		}
		else if (iParam1 < 85)
		{
			iVar0 = (8162 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 65) * (1456 - 1411)));
		}
		else if (iParam1 >= 85 && iParam1 <= 87)
		{
			iVar0 = ((iParam1 - 85) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 9062;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 9063;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 9064;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 9065;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 9066;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 9067;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 9068;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 9069;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 9070;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 9071;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 9072;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 9073;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 9074;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 9075;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 9076;
			}
		}
		else if (iParam1 < func_1120(11))
		{
			iVar0 = (11234 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - func_1120(7)) * (1456 - 1411)));
		}
		else if (iParam1 <= 157)
		{
			iVar0 = (11234 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - func_1120(7)) * (1456 - 1411)));
		}
		else if (iParam1 == 158)
		{
			iVar0 = (15306 - 1411);
			iVar0 = (iVar0 + iParam0);
		}
		else if (iParam1 < 184)
		{
			iVar0 = (16661 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 159) * (1456 - 1411)));
		}
		else if (iParam1 < 194)
		{
			iVar0 = (18403 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 184) * (1456 - 1411)));
		}
		else if (iParam1 < 231)
		{
			iVar0 = (19907 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 194) * (1456 - 1411)));
		}
		else if (iParam1 < 261)
		{
			iVar0 = (22915 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 231) * (1456 - 1411)));
		}
		else if (iParam1 < 271)
		{
			iVar0 = (25779 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 261) * (1456 - 1411)));
		}
		else if (iParam1 < 281)
		{
			iVar0 = (27499 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 271) * (1456 - 1411)));
		}
		else if (iParam1 < 291)
		{
			iVar0 = (28484 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 281) * (1456 - 1411)));
		}
		else if (iParam1 >= 291 && iParam1 <= 293)
		{
			iVar0 = ((iParam1 - 291) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 28934;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 28935;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 28936;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 28937;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 28938;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 28939;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 28940;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 28941;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 28942;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 28943;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 28944;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 28945;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 28946;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 28947;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 28948;
			}
		}
		else if (iParam1 < 304)
		{
			iVar0 = (28979 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 294) * (1456 - 1411)));
		}
		else if (iParam1 >= 304 && iParam1 <= 306)
		{
			iVar0 = ((iParam1 - 304) * (1876 - 1861));
			if (iParam0 == 1411)
			{
				iVar0 += 29429;
			}
			else if (iParam0 == 1412)
			{
				iVar0 += 29430;
			}
			else if (iParam0 == 1441)
			{
				iVar0 += 29431;
			}
			else if (iParam0 == 1442)
			{
				iVar0 += 29432;
			}
			else if (iParam0 == 1443)
			{
				iVar0 += 29433;
			}
			else if (iParam0 == 1445)
			{
				iVar0 += 29434;
			}
			else if (iParam0 == 1447)
			{
				iVar0 += 29435;
			}
			else if (iParam0 == 1448)
			{
				iVar0 += 29436;
			}
			else if (iParam0 == 1449)
			{
				iVar0 += 29437;
			}
			else if (iParam0 == 1450)
			{
				iVar0 += 29438;
			}
			else if (iParam0 == 1451)
			{
				iVar0 += 29439;
			}
			else if (iParam0 == 1452)
			{
				iVar0 += 29440;
			}
			else if (iParam0 == 1453)
			{
				iVar0 += 29441;
			}
			else if (iParam0 == 1454)
			{
				iVar0 += 29442;
			}
			else if (iParam0 == 1455)
			{
				iVar0 += 29443;
			}
		}
		else if (iParam1 > 306 && iParam1 < 317)
		{
			iVar0 = (30708 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 307) * (1456 - 1411)));
		}
		else if (iParam1 >= 317 && iParam1 < 337)
		{
			iVar0 = (32476 - 1411);
			iVar0 = (iVar0 + iParam0);
			iVar0 = (iVar0 + ((iParam1 - 317) * (1456 - 1411)));
		}
		return iVar0;
	}

	int PersonalVehicleCustomisationIo::getPackedIntHighModTypeIndex() const
	{
		const int iParam0 = pv;

		int iVar0;

		iVar0 = (4424 - 4400);
		if (iParam0 < 10)
		{
			return (4400 + (iParam0 * iVar0));
		}
		else if (iParam0 > 12 && iParam0 < 23)
		{
			return (4640 + ((iParam0 - 13) * iVar0));
		}
		else if (iParam0 > 25 && iParam0 < 36)
		{
			return (4880 + ((iParam0 - 26) * iVar0));
		}
		else if (iParam0 > 38 && iParam0 < 49)
		{
			return (5120 + ((iParam0 - 39) * iVar0));
		}
		else if (iParam0 > 51 && iParam0 < 62)
		{
			return (6414 + ((iParam0 - 52) * iVar0));
		}
		else if (iParam0 > 64 && iParam0 < 85)
		{
			return (7682 + ((iParam0 - 65) * iVar0));
		}
		else if (iParam0 >= 281 && iParam0 < 291)
		{
			return (29534 + ((iParam0 - 281) * iVar0));
		}
		else if (iParam0 >= 294 && iParam0 < 304)
		{
			return (29774 + ((iParam0 - 294) * iVar0));
		}
		else if (iParam0 < func_1120(11))
		{
			return (9554 + ((iParam0 - 88) * iVar0));
		}
		else if (iParam0 <= 157)
		{
			return (9554 + ((iParam0 - 88) * iVar0));
		}
		else if (iParam0 == 158)
		{
			return 15282;
		}
		else if (iParam0 < 184)
		{
			return (16061 + ((iParam0 - 159) * iVar0));
		}
		else if (iParam0 < 194)
		{
			return (18163 + ((iParam0 - 184) * iVar0));
		}
		else if (iParam0 < 231)
		{
			return (19019 + ((iParam0 - 194) * iVar0));
		}
		else if (iParam0 < 261)
		{
			return (22195 + ((iParam0 - 231) * iVar0));
		}
		else if (iParam0 < 271)
		{
			return (25539 + ((iParam0 - 261) * iVar0));
		}
		else if (iParam0 < 281)
		{
			return (27259 + ((iParam0 - 271) * iVar0));
		}
		else if (iParam0 > 306 && iParam0 < 317)
		{
			return (31188 + ((iParam0 - 307) * iVar0));
		}
		else if (iParam0 >= 317 && iParam0 < 337)
		{
			return (33436 + ((iParam0 - 317) * iVar0));
		}
		return 0;
	}

	int PersonalVehicleCustomisationIo::getPackedIntInteriorColourIndex() const
	{
		const int iParam0 = pv;

		if (iParam0 < 10)
		{
			return (5935 + iParam0);
		}
		else if (iParam0 > 12 && iParam0 < 23)
		{
			return (5935 + (iParam0 - 3));
		}
		else if (iParam0 > 25 && iParam0 < 36)
		{
			return (5935 + (iParam0 - 6));
		}
		else if (iParam0 > 38 && iParam0 < 49)
		{
			return (5935 + (iParam0 - 9));
		}
		else if (iParam0 > 51 && iParam0 < 62)
		{
			return ((7232 + iParam0) - 52);
		}
		else if (iParam0 > 64 && iParam0 < 85)
		{
			return ((9270 + iParam0) - 65);
		}
		else if (iParam0 >= 281 && iParam0 < 291)
		{
			return ((30094 + iParam0) - 281);
		}
		else if (iParam0 >= 294 && iParam0 < 304)
		{
			return (30094 + ((iParam0 - 281) - 3));
		}
		else if (iParam0 < func_1120(11))
		{
			return ((14944 + iParam0) - 88);
		}
		else if (iParam0 <= 157)
		{
			return ((14944 + iParam0) - 88);
		}
		else if (iParam0 == 158)
		{
			return 15359;
		}
		else if (iParam0 < 184)
		{
			return ((17986 + iParam0) - 159);
		}
		else if (iParam0 < 194)
		{
			return ((18933 + iParam0) - 184);
		}
		else if (iParam0 < 231)
		{
			return ((21868 + iParam0) - 194);
		}
		else if (iParam0 < 261)
		{
			return ((24475 + iParam0) - 231);
		}
		else if (iParam0 < 271)
		{
			return ((26299 + iParam0) - 261);
		}
		else if (iParam0 < 281)
		{
			return ((28019 + iParam0) - 271);
		}
		else if (iParam0 > 306 && iParam0 < 317)
		{
			return ((31468 + iParam0) - 307);
		}
		else if (iParam0 >= 317 && iParam0 < 337)
		{
			return ((33996 + iParam0) - 317);
		}
		return 0;
	}

	int PersonalVehicleCustomisationIo::getPackedIntRoofLiveryIndex() const
	{
		const int iParam0 = pv;

		if (iParam0 < 10)
		{
			return (7263 + iParam0);
		}
		else if (iParam0 > 12 && iParam0 < 23)
		{
			return (7263 + (iParam0 - 3));
		}
		else if (iParam0 > 25 && iParam0 < 36)
		{
			return (7263 + (iParam0 - 6));
		}
		else if (iParam0 > 38 && iParam0 < 49)
		{
			return (7263 + (iParam0 - 9));
		}
		else if (iParam0 > 51 && iParam0 < 62)
		{
			return ((7263 + iParam0) - 12);
		}
		else if (iParam0 > 64 && iParam0 < 85)
		{
			return ((9330 + iParam0) - 65);
		}
		else if (iParam0 >= 281 && iParam0 < 291)
		{
			return ((30154 + iParam0) - 281);
		}
		else if (iParam0 >= 294 && iParam0 < 304)
		{
			return (30154 + ((iParam0 - 281) - 3));
		}
		else if (iParam0 < func_1120(11))
		{
			return ((15154 + iParam0) - 88);
		}
		else if (iParam0 <= 157)
		{
			return ((15154 + iParam0) - 88);
		}
		else if (iParam0 == 158)
		{
			return 15362;
		}
		else if (iParam0 < 184)
		{
			return ((18061 + iParam0) - 159);
		}
		else if (iParam0 < 194)
		{
			return ((18963 + iParam0) - 184);
		}
		else if (iParam0 < 231)
		{
			return ((21979 + iParam0) - 194);
		}
		else if (iParam0 < 261)
		{
			return ((24565 + iParam0) - 231);
		}
		else if (iParam0 < 271)
		{
			return ((26329 + iParam0) - 261);
		}
		else if (iParam0 < 281)
		{
			return ((28049 + iParam0) - 271);
		}
		else if (iParam0 > 306 && iParam0 < 317)
		{
			return ((31528 + iParam0) - 307);
		}
		else if (iParam0 >= 317 && iParam0 < 337)
		{
			return ((34056 + iParam0) - 317);
		}
		return 0;
	}

	int PersonalVehicleCustomisationIo::getPackedIntDashboardColourIndex() const
	{
		const int iParam0 = pv;

		if (iParam0 < 10)
		{
			return (5975 + iParam0);
		}
		else if (iParam0 > 12 && iParam0 < 23)
		{
			return (5975 + (iParam0 - 3));
		}
		else if (iParam0 > 25 && iParam0 < 36)
		{
			return (5975 + (iParam0 - 6));
		}
		else if (iParam0 > 38 && iParam0 < 49)
		{
			return (5975 + (iParam0 - 9));
		}
		else if (iParam0 > 51 && iParam0 < 62)
		{
			return ((7242 + iParam0) - 52);
		}
		else if (iParam0 > 64 && iParam0 < 85)
		{
			return ((9290 + iParam0) - 65);
		}
		else if (iParam0 >= 281 && iParam0 < 291)
		{
			return ((30114 + iParam0) - 281);
		}
		else if (iParam0 >= 294 && iParam0 < 304)
		{
			return (30114 + ((iParam0 - 281) - 3));
		}
		else if (iParam0 < func_1120(11))
		{
			return ((15014 + iParam0) - 88);
		}
		else if (iParam0 <= 157)
		{
			return ((15014 + iParam0) - 88);
		}
		else if (iParam0 == 158)
		{
			return 15360;
		}
		else if (iParam0 < 184)
		{
			return ((18011 + iParam0) - 159);
		}
		else if (iParam0 < 194)
		{
			return ((18943 + iParam0) - 184);
		}
		else if (iParam0 < 231)
		{
			return ((21905 + iParam0) - 194);
		}
		else if (iParam0 < 261)
		{
			return ((24505 + iParam0) - 231);
		}
		else if (iParam0 < 271)
		{
			return ((26309 + iParam0) - 261);
		}
		else if (iParam0 < 281)
		{
			return ((28029 + iParam0) - 271);
		}
		else if (iParam0 > 306 && iParam0 < 317)
		{
			return ((31478 + iParam0) - 307);
		}
		else if (iParam0 >= 317 && iParam0 < 337)
		{
			return ((34016 + iParam0) - 317);
		}
		return 0;
	}

	int PersonalVehicleCustomisationIo::getPackedIntTyresModeIndex() const
	{
		const int iParam0 = pv;
		constexpr bool bParam1 = false;

		if (iParam0 <= 38)
		{
			if (iParam0 < 10)
			{
				if (bParam1)
				{
					return (4212 + iParam0);
				}
				else
				{
					return (31488 + iParam0);
				}
			}
			else if (iParam0 > 12 && iParam0 < 23)
			{
				if (bParam1)
				{
					return ((4212 + iParam0) - 3);
				}
				else
				{
					return ((31488 + iParam0) - 3);
				}
			}
			else if (iParam0 > 25 && iParam0 < 36)
			{
				if (bParam1)
				{
					return ((4212 + iParam0) - 6);
				}
				else
				{
					return ((31488 + iParam0) - 6);
				}
			}
		}
		else if (iParam0 > 38 && iParam0 < 49)
		{
			return ((6015 + iParam0) - 39);
		}
		else if (iParam0 > 51 && iParam0 < 62)
		{
			return ((7252 + iParam0) - 52);
		}
		else if (iParam0 > 64 && iParam0 < 85)
		{
			return ((9310 + iParam0) - 65);
		}
		else if (iParam0 >= 281 && iParam0 < 291)
		{
			return ((30134 + iParam0) - 281);
		}
		else if (iParam0 >= 294 && iParam0 < 304)
		{
			return (30134 + ((iParam0 - 281) - 3));
		}
		else if (iParam0 < func_1120(11))
		{
			return ((15084 + iParam0) - 88);
		}
		else if (iParam0 <= 157)
		{
			return ((15084 + iParam0) - 88);
		}
		else if (iParam0 == 158)
		{
			return 15361;
		}
		else if (iParam0 < 184)
		{
			return ((18036 + iParam0) - 159);
		}
		else if (iParam0 < 194)
		{
			return ((18953 + iParam0) - 184);
		}
		else if (iParam0 < 231)
		{
			return ((21942 + iParam0) - 194);
		}
		else if (iParam0 < 261)
		{
			return ((24535 + iParam0) - 231);
		}
		else if (iParam0 < 271)
		{
			return ((26319 + iParam0) - 261);
		}
		else if (iParam0 < 281)
		{
			return ((28039 + iParam0) - 271);
		}
		else if (iParam0 > 306 && iParam0 < 317)
		{
			return ((31488 + 30 + iParam0) - 307);
		}
		else if (iParam0 >= 317 && iParam0 < 337)
		{
			return ((34036 + iParam0) - 317);
		}
		return 0;
	}

	int PersonalVehicleCustomisationIo::extraToFlagsIndex(const int iParam0)
	{
		switch (iParam0)
		{
		case 1:
			return 0;
			break;

		case 2:
			return 1;
			break;

		case 3:
			return 2;
			break;

		case 4:
			return 3;
			break;

		case 5:
			return 4;
			break;

		case 6:
			return 5;
			break;

		case 7:
			return 6;
			break;

		case 8:
			return 7;
			break;

		case 9:
			return 8;
			break;

		case 10:
			return 24;
			break;

		case 11:
			return 25;
			break;

		case 12:
			return 26;
			break;
		}
		return 0;
	}
}
