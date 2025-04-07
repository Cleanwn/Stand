#include "CommandListRadio.hpp"

#include "CommandListRadioStations.hpp"
#include "CommandMobileRadio.hpp"
#include "CommandLockRadio.hpp"

namespace Stand
{
	CommandListRadio::CommandListRadio(CommandList* const parent)
		: CommandList(parent, LOC("RD"))
	{
		this->createChild<CommandListRadioStations>();
		this->createChild<CommandMobileRadio>();
		this->createChild<CommandLockRadio>();
	}
}
