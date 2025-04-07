#include "CommandListSessionSpoof.hpp"

#include "CommandHideSession.hpp"
#include "CommandSpoofSessionType.hpp"
#include "CommandSpoofHost.hpp"
#include "CommandSpoofedHost.hpp"

namespace Stand
{
	CommandListSessionSpoof::CommandListSessionSpoof(CommandList* parent)
		: CommandList(parent, LOC("SPFSESS"))
	{
		this->createChild<CommandHideSession>();
		this->createChild<CommandSpoofSessionType>();
		{
			auto host = this->createChild<CommandList>(LOC("FLAG_HOST"));

			host->createChild<CommandSpoofHost>();
			host->createChild<CommandSpoofedHost>();
		}
	}
}
