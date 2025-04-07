#include "create_rainbow.hpp"

#include <fmt/core.h>

#include "CommandColourCustom.hpp"
#include "CommandRainbow.hpp"

namespace Stand
{
	void create_rainbow(CommandColourCustom* colour)
	{
		colour->children.insert(colour->children.begin(), colour->makeChild<CommandRainbow>(LOC("LGBT"), colour->getRainbowCommandNames(), colour));
	}
}
