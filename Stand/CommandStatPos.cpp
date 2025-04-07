#include "CommandStatPos.hpp"

#include "CommandStatPosField.hpp"

namespace Stand
{
	CommandStatPos::CommandStatPos(CommandList* const parent, Stat& stat)
		: CommandList(parent, stat.getLabel(), {}), stat(stat)
	{
		createChild<CommandStatPosField>(PosType::X, stat);
		createChild<CommandStatPosField>(PosType::Y, stat);
		createChild<CommandStatPosField>(PosType::Z, stat);

		useCommandNamesFromMenuName();
	}
}