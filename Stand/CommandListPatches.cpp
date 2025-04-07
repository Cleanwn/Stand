#include "CommandListPatches.hpp"

#include "CommandTogglePointer.hpp"
#include "Hooking.hpp"

namespace Stand
{
	CommandListPatches::CommandListPatches(CommandList* const parent)
		: CommandList(parent, LOC("PTCHS"))
	{
		this->createChild<CommandTogglePointer>(&g_hooking.patch_projectile_pool, LOC("PRJTPOOL"), {}, LOC("PTCH_PRJTPOOL_H"));
	}
}
