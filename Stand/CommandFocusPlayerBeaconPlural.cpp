#include "CommandFocusPlayerBeaconPlural.hpp"

#include "Gui.hpp"

namespace Stand
{
	CommandFocusPlayerBeaconPlural::CommandFocusPlayerBeaconPlural(CommandList* const parent)
		: CommandToggle(parent, LOC("ARBCNPLY_P"))
	{
	}

	void CommandFocusPlayerBeaconPlural::onChange(Click& click)
	{
		g_gui.focused_player_beacon_in_list_of_all_players = m_on;
	}
}
