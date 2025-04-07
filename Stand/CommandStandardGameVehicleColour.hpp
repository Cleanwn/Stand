#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandStandardGameVehicleColour : public CommandList
	{
	public:
		enum ColourType : uint8_t
		{
			WHEELS,
			PRIMARY,
			SECONDARY,
			PEARLESCENT,
			INTERIOR,
		};

		explicit CommandStandardGameVehicleColour(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, ColourType colour_type);
	};
}