#include "CommandStandardGameVehicleColour.hpp"

#include <soup/macros.hpp> // COUNT

#include "CommandSearchLiterals.hpp"
#include "CommandStandardGameVehicleColourItem.hpp"

namespace Stand
{
	CommandStandardGameVehicleColour::CommandStandardGameVehicleColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, ColourType colour_type)
		: CommandList(parent, std::move(menu_name), std::move(command_names))
	{
		createChild<CommandSearchLiterals<false>>(combineCommandNames(CMDNAMES("search"), this->command_names));

		for (uint8_t i = 0; i != COUNT(standard_vehicle_colours); ++i)
		{
			createChild<CommandStandardGameVehicleColourItem>(command_names, i, colour_type);
		}
	}
}