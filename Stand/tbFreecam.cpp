#include "tbFreecam.hpp"

#include "AbstractEntity.hpp"
#include "ButtonInstructions.hpp"
#include "Camgr.hpp"
#include "CommandSlider.hpp"
#include "CommandToggleNoCorrelation.hpp"
#include "free_movement.hpp"
#include "gta_input.hpp"
#include "natives.hpp"
#include "unrestrictGameplayCam.hpp"

namespace Stand
{
	bool tbFreecam::canMovementCommandPerformMovement() const noexcept
	{
		return !isEnabled() || !movement->m_on;
	}

	void tbFreecam::ensureMovementControlsAreEnabled() const
	{
		Click click(CLICK_AUTO, TC_OTHER);
		movement->setStateBool(click, true);
	}

	void tbFreecam::ensureNotInVehicleFirstPerson(int set_cam_to)
	{
		if (g_player_veh.isValid())
		{
			auto cam_ctx = CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT();
			if (CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(cam_ctx) == 4)
			{
				CAM::SET_CAM_VIEW_MODE_FOR_CONTEXT(cam_ctx, set_cam_to);
			}
		}
	}

	void tbFreecam::onEnable()
	{
		pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
		tbToggleableBehaviourScript::onEnable();
	}

	void tbFreecam::onDisable()
	{
		HUD::UNLOCK_MINIMAP_POSITION();
		Camgr::unlock_pos();
		Camgr::unlock_focus();
	}

	void tbFreecam::onTick()
	{
		ensureNotInVehicleFirstPerson(2);
		unrestrictGameplayCam();
		v3 rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
		if (movement->m_on)
		{
			if (show_button_instructions->m_on)
			{
				ButtonInstructions::setFreeMovementThisTick();
			}
			for (const int input : g_movement_inputs)
			{
				PAD::DISABLE_CONTROL_ACTION(0, input, true);
			}
			if (ignore_pitch->m_on)
			{
				rot.x = 0.0f;
			}
			free_movement(pos, rot, ((float)speed->value / 100.0f), ((float)sprint_speed->value / 100.0f));
		}
		Camgr::lock_pos(pos);
		if (Camgr::freecam_focus_allowed)
		{
			Camgr::lock_focus(pos);
		}
		if (minimap_follows->m_on)
		{
			minimap_needs_cleanup = true;
			HUD::LOCK_MINIMAP_POSITION(pos.x, pos.y);
		}
		else
		{
			minimapCleanup();
		}
	}

	void tbFreecam::minimapCleanup()
	{
		if (minimap_needs_cleanup)
		{
			minimap_needs_cleanup = false;
			HUD::UNLOCK_MINIMAP_POSITION();
		}
	}
}
