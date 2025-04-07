#include "CommandForceRelay.hpp"

#include "Hooking.hpp"
#include "pointers.hpp"

namespace Stand
{
	CommandForceRelay::CommandForceRelay(CommandList* const parent)
		: CommandToggle(parent, LOC("FRCRLY"), CMDNAMES_OBF("forcerelayconnections"))
	{
	}

	void CommandForceRelay::onEnable(Click& click)
	{
		g_hooking.force_relay_connections = true;
		if (pointers::netIceSession_terminationTimer_IsTimedOut != nullptr)
		{
			patch.initPatchNOP(pointers::netIceSession_terminationTimer_IsTimedOut, 2 + 4 + 2);
		}
	}

	void CommandForceRelay::onDisable(Click& click)
	{
		g_hooking.force_relay_connections = false;
		patch.restore();
	}
}
