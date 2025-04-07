#include "CommandCustomGlider.hpp"

#include "CommandSlider.hpp"

#include "CommandCustomGliderSetspeed.hpp"
#include "CommandCustomGliderToggle.hpp"

namespace Stand
{
	CommandCustomGlider::CommandCustomGlider(CommandList* const parent)
		: CommandList(parent, LOC("CSTWNGBVHR"), {}, LOC("CSTWNGBVHR_H"))
	{
		this->createChild<CommandCustomGliderToggle>();
		min = this->createChild<CommandSlider>(LOC("MINPOS"), CMDNAMES("wingsmin"), NOLABEL, -100000, 100000, 0, 5);
		max = this->createChild<CommandSlider>(LOC("MAXPOS"), CMDNAMES("wingsmax"), NOLABEL, -100000, 100000, 15, 5);
		speed = this->createChild<CommandSlider>(LOC("CSTWNGBVHR_SPD"), CMDNAMES("wingsspeed", "wingspeed"), NOLABEL, 0, 1000000, 5, 1);
		this->createChild<CommandCustomGliderSetspeed>();
	}
}
