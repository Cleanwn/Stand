#pragma once

#include "CommandAction.hpp"

#include "AbstractEntity.hpp"
#include "CommandStandardGameVehicleColour.hpp"
#include "StringUtils.hpp"
#include "Util.hpp"
#include "vehicle_colours.hpp"

namespace Stand
{
	class CommandStandardGameVehicleColourItem : public CommandAction
	{
	private:
		using ColourType = CommandStandardGameVehicleColour::ColourType;

		const ColourType colour_type;
		const uint8_t index;

	public:
		explicit CommandStandardGameVehicleColourItem(CommandList* const parent, std::vector<CommandName> command_names, uint8_t colour, CommandStandardGameVehicleColour::ColourType colour_type)
			: CommandAction(parent, Label(standard_vehicle_colours[colour]), std::move(command_names)), index(colour), colour_type(colour_type)
		{
			menu_name.makeLiteralLocalised(); // for CommandSearchLiterals

			if (index != 0)
			{
				flags |= CMDFLAG_FEATURELIST_FINISHLIST;
			}
		}

		void onClick(Click& click) final
		{
			if (auto veh = Util::getVehicle(); veh.isValid())
			{
				switch (colour_type)
				{
				case ColourType::WHEELS:
					{
						int p, w;
						VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &p, &w);
						VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, p, index);
					}
					break;

				case ColourType::PRIMARY:
					{
						VEHICLE::CLEAR_VEHICLE_CUSTOM_PRIMARY_COLOUR(veh);
						veh.setPrimaryColour(index);
					}
					break;

				case ColourType::SECONDARY:
					{
						VEHICLE::CLEAR_VEHICLE_CUSTOM_SECONDARY_COLOUR(veh);
						veh.setSecondaryColour(index);
					}
					break;
					 
				case ColourType::PEARLESCENT:
					{
						int p, w;
						VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &p, &w);
						VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, index, w);
					}
					break;

				case ColourType::INTERIOR:
					{
						// Colour var 5 also refers to:
						//   - Windsor2 roof
						//   - The threading on some vehicles.
						//   - Rollbar colours on a fair amount of vehicles. (pretty common)
						//   - Dial border colours on some vehicles.
						//   - The frames of some trailers. (excluded)
						//   - The center line on a racing steering wheel.
						//   - A small portion of the steering bar on motorcycles.
						//   - Steering wheel colour.
						//
						// All except those expressly noted are fairly rare and only appear on a vehicle or two.
						//
						VEHICLE::SET_VEHICLE_EXTRA_COLOUR_5(veh, index);
					}
					break;
				}
			}
		}
	};
}