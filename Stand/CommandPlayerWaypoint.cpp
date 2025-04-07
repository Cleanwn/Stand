#include "CommandPlayerWaypoint.hpp"

#include "PlayerProvider.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandPlayerWaypoint::CommandPlayerWaypoint(CommandList* const parent)
		: CommandOnPlayer<CommandToggle>(parent, LOC("WP"), CMDNAMES_OBF("wp", "wpon", "waypoint", "waypointon"), NOLABEL, false, CMDFLAGS_TOGGLE | CMDFLAG_TEMPORARY)
	{
		PC_PTR->registerCommandNames(this);
	}

	void CommandPlayerWaypoint::onEnable(Click& click)
	{
		const AbstractPlayer p = PP_PTR->getPlayers(true).at(0);
		p.waypoint();
		ensureYieldableScriptThread(click, [this, p]
		{
			if (dedup)
			{
				dedup = false;
				Script::current()->yield();
			}
			dedup = true;
			registerScriptTickEventHandler(TC_SCRIPT_YIELDABLE, [this, p]()
			{
				if (m_on && dedup && p.exists())
				{
					p.waypoint();
					return true;
				}
				if (m_on)
				{
					m_on = false;
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					updateState(click);
				}
				HUD::SET_WAYPOINT_OFF();
				return false;
			});
		});
	}
}