#include "CommandFocusPlayerBeaconSingular.hpp"

#include "Gui.hpp"

namespace Stand
{
	CommandFocusPlayerBeaconSingular::CommandFocusPlayerBeaconSingular(CommandList* const parent)
		: CommandToggle(parent, LOC("ARBCNPLY_S"))
	{
	}

	void CommandFocusPlayerBeaconSingular::onChange(Click& click)
	{
		g_gui.focused_player_beacon_in_list_belonging_to_player = m_on;
	}
}
