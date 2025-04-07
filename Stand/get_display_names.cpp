#include "get_display_names.hpp"

#include "AptHelper.hpp"
#include "get_vehicle_display_name.hpp"
#include "radio_stations.hpp"
#include "Util.hpp"
#include "vehicle_items.hpp"
#include "weapons.hpp"
#include "weapon_components.hpp"

namespace Stand
{
	std::set<std::string> get_display_names()
	{
		std::set<std::string> display_names{};
		for (uint8_t i = 0; i < AptHelper::max_property_id; ++i)
		{
			display_names.emplace(AptHelper::propertyIdToName(i));
		}
		for (const auto& radio_station : radio_stations)
		{
			display_names.emplace(Util::GET_LABEL_TEXT(radio_station, false));
		}
		for (const auto& vehicle_item : vehicle_items)
		{
			display_names.emplace(get_vehicle_display_name(&vehicle_item));
		}
		for(uint8_t i = 0; i < Weapon::_NUM_CATEGORIES; ++i)
		{
			display_names.emplace(Weapon::getCategoryName((Weapon::Category)i));
		}
		for (const auto& weapon : Weapon::weapons)
		{
			display_names.emplace(weapon.getDisplayName());
		}
		for (const auto& comp : g_weapon_components)
		{
			if (comp.nameIsLabelKey())
			{
				display_names.emplace(comp.getName().getLiteralUtf8());
			}
		}
		return display_names;
	}
}
