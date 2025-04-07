#include "CommandFocusPlayerBeaconExcludeMe.hpp"

#include "Gui.hpp"

namespace Stand
{
	CommandFocusPlayerBeaconExcludeMe::CommandFocusPlayerBeaconExcludeMe(CommandList* const parent)
		: CommandToggle(parent, LOC("XCLDM"), {}, NOLABEL, true)
	{
	}

	void CommandFocusPlayerBeaconExcludeMe::onChange(Click& click)
	{
		g_gui.focused_player_beacon_exclude_me = m_on;
	}
}
