#include "get_vehicle_display_name.hpp"

#include "atStringHash.hpp"
#include "CVehicleModelInfo.hpp"
#include "joaatToString.hpp"
#include "natives.hpp"
#include "Util.hpp"

namespace Stand
{
	std::string get_vehicle_display_name_no_manufacturer(hash_t model)
	{
		if (NativeCallContext::canInvoke(0xB215AAC32D25D019))
		{
			auto text = Util::GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(model), false);
			if (text != "NULL")
			{
				return text;
			}
		}
		return joaatToString(model);
	}

	std::string get_vehicle_display_name_no_manufacturer(const char* modelName)
	{
		if (NativeCallContext::canInvoke(0xB215AAC32D25D019))
		{
			auto text = Util::GET_LABEL_TEXT(VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(rage::atStringHash(modelName)), false);
			if (text != "NULL")
			{
				return text;
			}
		}
		return modelName;
	}

	std::string get_vehicle_display_name_no_manufacturer(const VehicleItem* veh)
	{
		return get_vehicle_display_name_no_manufacturer(veh->name);
	}

	std::string get_vehicle_display_name(const VehicleItem* veh)
	{
		if (veh->manufacturer[0] != 0)
		{
			std::string name{};
			name = Util::GET_LABEL_TEXT(veh->manufacturer, false);
			if (name != "NULL")
			{
				name.push_back(' ');
				name.append(get_vehicle_display_name_no_manufacturer(veh));
				return name;
			}
		}
		return get_vehicle_display_name_no_manufacturer(veh);
	}

	std::string get_vehicle_display_name(const CVehicleModelInfo* veh)
	{
		if (veh->manufacturer[0] != 0)
		{
			std::string name{};
			name = Util::GET_LABEL_TEXT(veh->manufacturer, false);
			if (name != "NULL")
			{
				name.push_back(' ');
				name.append(Util::getLabelTextOrKey(veh->name));
				return name;
			}
		}
		return Util::getLabelTextOrKey(veh->name);
	}
}
