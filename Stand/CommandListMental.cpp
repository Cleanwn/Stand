#include "CommandListMental.hpp"

#include "CommandMental.hpp"
#include "CommandLockMentalState.hpp"
#include "CommandMentalPulse.hpp"

namespace Stand
{
	CommandListMental::CommandListMental(CommandList* const parent)
		: CommandList(parent, LOC("MNTLS"))
	{
		mental = this->createChild<CommandMental>();
		this->createChild<CommandLockMentalState>();
		pulse = this->createChild<CommandMentalPulse>();
	}
}
