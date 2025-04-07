#include "CommandPlayerSpectateNinja.hpp"

#include "AbstractEntity.hpp"
#include "Camgr.hpp"
#include "natives.hpp"
#include "PlayerProvider.hpp"
#include "Script.hpp"
#include "SpectateMgr.hpp"

namespace Stand
{
	CommandPlayerSpectateNinja::CommandPlayerSpectateNinja(CommandList* const parent)
		: CommandPlayerSpectate(parent, LOC("JM_NUTS"), CMDNAMES_OBF("spectate", "spyon"))
	{
		// If menu_name is changed, also update in [When Entering Player's List... > Spectate Them]
	}

	void CommandPlayerSpectateNinja::onEnable(Click& click)
	{
		const AbstractPlayer p = PP_PTR->getPlayers(true).at(0);
		ensureYieldableScriptThread(click, [this, p]
		{
			if (dedup)
			{
				dedup = false;
				Script::current()->yield();
			}
			dedup = true;
			Camgr::stopAnyActiveCamCommand();
			registerScriptTickEventHandler(TC_SCRIPT_YIELDABLE, [this, p]()
			{
				const bool can_run = (m_on && dedup);
				if (can_run && p.exists())
				{
					SpectateMgr::onTickSpectating(p.getPed());
					CAM::SET_GAMEPLAY_CAM_FOLLOW_PED_THIS_UPDATE(g_cam_ped);
					CAM::INVALIDATE_CINEMATIC_VEHICLE_IDLE_MODE();
					CAM::INVALIDATE_IDLE_CAM();
					auto pos = g_cam_ped.getPos();
					Camgr::lock_focus(pos);
					HUD::SET_MINIMAP_IN_SPECTATOR_MODE(TRUE, g_cam_ped);
				}
				else
				{
					SpectateMgr::onStopSpectating();
					Camgr::unlock_focus();
					HUD::SET_MINIMAP_IN_SPECTATOR_MODE(FALSE, 0);
				}
				if (can_run)
				{
					SpectateMgr::onTickSpectating();
					return true;
				}
				if (m_on)
				{
					m_on = false;
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					updateState(click);
				}
				return false;
			});
		});
	}
}