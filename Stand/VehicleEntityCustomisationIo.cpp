#include "VehicleEntityCustomisationIo.hpp"

#include <utility>

#include "AbstractModel.hpp"
#include "natives.hpp"
#include "VehicleColour.hpp"

namespace Stand
{
	Mixed VehicleEntityCustomisationIo::read(int idx)
	{
		if (isVehicleMod(idx))
		{
			idx = translatePropertyToVehicleMod(idx);
			if (idx == VehicleMods::livery && VEHICLE::GET_NUM_VEHICLE_MODS(veh, VehicleMods::livery) == 0)
			{
				return VEHICLE::GET_VEHICLE_LIVERY(veh);
			}
			if (idx == VehicleMods::xenon_lights)
			{
				return veh.getHeadlightsVariation();
			}
			if (VehicleMods::isBooleanExcludeLights(idx))
			{
				return (bool)VEHICLE::IS_TOGGLE_MOD_ON(veh, idx);
			}
			auto val = VEHICLE::GET_VEHICLE_MOD(veh, idx) + 1;
			if (VehicleMods::hasVariation(idx))
			{
				return std::pair<int, bool>(val, VEHICLE::GET_VEHICLE_MOD_VARIATION(veh, idx));
			}
			return val;
		}
		switch (idx)
		{
		case MODEL:
			return veh.getModel().operator Hash();

		case PLATE_STYLE:
			return VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(veh);

		case PLATE_TEXT:
			if (const char* plate_text = VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(veh))
			{
				return plate_text;
			}
			return "";

		case PRIMARY_COLOUR:
			return veh.getPrimaryColour();

		case CUSTOM_PRIMARY_COLOUR:
		{
			OptVehicleColour c{};
			if (VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(veh))
			{
				c.is_applicable = true;
				VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, &c.colour.r, &c.colour.g, &c.colour.b);
			}
			return c;
		}

		case SECONDARY_COLOUR:
			return veh.getSecondaryColour();

		case CUSTOM_SECONDARY_COLOUR:
		{
			OptVehicleColour c{};
			if (VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(veh))
			{
				c.is_applicable = true;
				VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, &c.colour.r, &c.colour.g, &c.colour.b);
			}
			return c;
		}

		case PEARL_COLOUR:
		{
			int pearl;
			int wheel;
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &pearl, &wheel);
			return pearl;
		}

		case WHEEL_COLOUR:
		{
			int pearl;
			int wheel;
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &pearl, &wheel);
			return wheel;
		}
			
		case ENVEFF_SCALE:
			return VEHICLE::GET_VEHICLE_ENVEFF_SCALE(veh);

		case INTERIOR_COLOUR:
		{
			int colour;
			VEHICLE::GET_VEHICLE_EXTRA_COLOUR_5(veh, &colour);
			return colour;
		}

		case ROOF_LIVERY:
			return VEHICLE::GET_VEHICLE_LIVERY2(veh);

		case DASHBOARD_COLOUR:
		{
			int colour;
			VEHICLE::GET_VEHICLE_EXTRA_COLOUR_6(veh, &colour);
			return colour;
		}

		case TYRES_MODE:
		{
			if (VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(veh))
			{
				if (VEHICLE::GET_DRIFT_TYRES_SET(veh))
				{
					return int(3);
				}
				else
				{
					return int(2);
				}
			}
			else
			{
				if (VEHICLE::GET_DRIFT_TYRES_SET(veh))
				{
					return int(4); // not a real value recognized by the game scripts
				}
				else
				{
					return int(1);
				}
			}
			break;
		}

		case TYRE_SMOKE_COLOUR:
		{
			VehicleColour colour{};
			VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(veh, &colour.r, &colour.g, &colour.b);
			return colour;
		}

		case WINDOW_TINT:
		{
			int val = VEHICLE::GET_VEHICLE_WINDOW_TINT(veh);
			if (val == -1 && !veh.getModel().hasWindowTintMinusOne())
			{
				val = 0;
			}
			return val;
		}

		case NEON_COLOUR:
		{
			VehicleColour colour{};
			VEHICLE::GET_VEHICLE_NEON_COLOUR(veh, &colour.r, &colour.g, &colour.b);
			return colour;
		}
		
		case NEON_FRONT:
			return (bool)VEHICLE::GET_VEHICLE_NEON_ENABLED(veh, 2);

		case NEON_BACK:
			return (bool)VEHICLE::GET_VEHICLE_NEON_ENABLED(veh, 3);

		case NEON_LEFT:
			return (bool)VEHICLE::GET_VEHICLE_NEON_ENABLED(veh, 0);

		case NEON_RIGHT:
			return (bool)VEHICLE::GET_VEHICLE_NEON_ENABLED(veh, 1);

		case WHEEL_TYPE:
			return VEHICLE::GET_VEHICLE_WHEEL_TYPE(veh);

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
		case EXTRA_13:
		case EXTRA_14:
		case EXTRA_15:
			return (bool)VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(veh, ((idx - EXTRA_1) + 1));
		}
		return -1;
	}

	void VehicleEntityCustomisationIo::write(int idx, Mixed&& value)
	{
		if (isVehicleMod(idx))
		{
			idx = translatePropertyToVehicleMod(idx);
			if (idx == VehicleMods::xenon_lights)
			{
				veh.setHeadlightsVariation(value);
			}
			else if (VehicleMods::hasVariation(idx))
			{
				auto val = value.as<std::pair<int, bool>>();
				if (VEHICLE::GET_VEHICLE_MOD(veh, idx) != val->first - 1)
				{
					VEHICLE::REMOVE_VEHICLE_MOD(veh, idx);
					if (val->first > 0)
					{
						VEHICLE::SET_VEHICLE_MOD(veh, idx, val->first - 1, val->second);
					}
				}
			}
			else if (VehicleMods::isBooleanExcludeLights(idx))
			{
				VEHICLE::TOGGLE_VEHICLE_MOD(veh, idx, value.get<bool>());
			}
			else
			{
				auto val = value.get<int>();
				if (idx == VehicleMods::livery && VEHICLE::GET_NUM_VEHICLE_MODS(veh, VehicleMods::livery) == 0)
				{
					VEHICLE::SET_VEHICLE_LIVERY(veh, value);
				}
				else if (VEHICLE::GET_VEHICLE_MOD(veh, idx) != val)
				{
					VEHICLE::REMOVE_VEHICLE_MOD(veh, idx);
					if (val > 0)
					{
						VEHICLE::SET_VEHICLE_MOD(veh, idx, val - 1, false);
					}
				}
			}
		}
		else switch (idx)
		{
		case PLATE_STYLE:
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(veh, value);
			break;

		case PLATE_TEXT:
			veh.setPlateText(value.get<const char*>());
			break;

		case PRIMARY_COLOUR:
			veh.setPrimaryColour(value);
			break;

		case CUSTOM_PRIMARY_COLOUR:
		{
			auto c = value.as<OptVehicleColour>();
			if (c->is_applicable)
			{
				VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh, c->colour.r, c->colour.g, c->colour.b);
			}
			else
			{
				VEHICLE::CLEAR_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh);
			}
			break;
		}

		case SECONDARY_COLOUR:
			veh.setSecondaryColour(value);
			break;

		case CUSTOM_SECONDARY_COLOUR:
		{
			auto c = value.as<OptVehicleColour>();
			if (c->is_applicable)
			{
				VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh, c->colour.r, c->colour.g, c->colour.b);
			}
			else
			{
				VEHICLE::CLEAR_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh);
			}
			break;
		}

		case PEARL_COLOUR:
		{
			int pearl;
			int wheel;
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &pearl, &wheel);
			VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, value, wheel);
			break;
		}

		case WHEEL_COLOUR:
		{
			int pearl;
			int wheel;
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &pearl, &wheel);
			VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, pearl, value);
			break;
		}

		case ENVEFF_SCALE:
			VEHICLE::SET_VEHICLE_ENVEFF_SCALE(veh, value);
			break;

		case INTERIOR_COLOUR:
			VEHICLE::SET_VEHICLE_EXTRA_COLOUR_5(veh, value);
			break;

		case ROOF_LIVERY:
			VEHICLE::SET_VEHICLE_LIVERY2(veh, value);
			break;

		case DASHBOARD_COLOUR:
			VEHICLE::SET_VEHICLE_EXTRA_COLOUR_6(veh, value);
			break;

		case TYRES_MODE:
		{
			auto val = value.get<int>();
			if (val == 1)
			{
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, false);
				VEHICLE::SET_DRIFT_TYRES(veh, false);
			}
			else if (val == 3)
			{
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, true);
				VEHICLE::SET_DRIFT_TYRES(veh, true);
			}
			else if (val == 4)
			{
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, false);
				VEHICLE::SET_DRIFT_TYRES(veh, true);
			}
			else
			{
				VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(veh, true);
				VEHICLE::SET_DRIFT_TYRES(veh, false);
			}
			break;
		}

		case WINDOW_TINT:
		{
			auto val = value.get<int>();
			VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, 0);
			if (val != -1 || veh.getModel().hasWindowTintMinusOne())
			{
				VEHICLE::SET_VEHICLE_WINDOW_TINT(veh, val);
			}
			break;
		}

		case TYRE_SMOKE_COLOUR:
		{
			auto val = value.as<VehicleColour>();
			VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(veh, val->r, val->g, val->b);
			break;
		}

		case NEON_COLOUR:
		{
			auto val = value.as<VehicleColour>();
			VEHICLE::SET_VEHICLE_NEON_COLOUR(veh, val->r, val->g, val->b);
			break;
		}

		case NEON_FRONT:
			VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, 2, value.get<bool>());
			break;

		case NEON_BACK:
			VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, 3, value.get<bool>());
			break;

		case NEON_LEFT:
			VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, 0, value.get<bool>());
			break;

		case NEON_RIGHT:
			VEHICLE::SET_VEHICLE_NEON_ENABLED(veh, 1, value.get<bool>());
			break;

		case WHEEL_TYPE:
			VEHICLE::SET_VEHICLE_WHEEL_TYPE(veh, value);
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
		case EXTRA_13:
		case EXTRA_14:
		case EXTRA_15:
			VEHICLE::SET_VEHICLE_EXTRA(veh, ((idx - EXTRA_1) + 1), !value.get<bool>());
			break;
		}
	}
}
