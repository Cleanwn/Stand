#include "CommandTrafficColour.hpp"

#include "CommandTrafficColourMain.hpp"
#include "create_colour_with_rainbow.hpp"

namespace Stand
{
	CommandTrafficColour::CommandTrafficColour(CommandList* const parent)
		: CommandList(parent, LOC("TRFCLR"))
	{
		auto main = this->createChild<CommandTrafficColourMain>();
		create_colour_with_rainbow(this, &main->colour, { CMDNAME("trafficcolour") });
	}
}
