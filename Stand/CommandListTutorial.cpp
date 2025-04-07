#include "CommandListTutorial.hpp"

#include "CommandRepeatTutorial.hpp"
#include "CommandDismissTutorials.hpp"

namespace Stand
{
	CommandListTutorial::CommandListTutorial(CommandList* const parent)
		: CommandList(parent, LOC("TUT"))
	{
		this->createChild<CommandRepeatTutorial>();
		this->createChild<CommandDismissTutorials>();
	}
}
