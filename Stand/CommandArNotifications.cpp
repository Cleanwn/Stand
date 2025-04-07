#include "CommandArNotifications.hpp"

#include "AbstractEntity.hpp"
#include "CommandListSelect.hpp"
#include "CommandSliderFloat.hpp"
#include "CommandToggleRegularSp.hpp"
#include "tbFreecam.hpp"
#include "pointers.hpp"
#include "tbScreenshotMode.hpp"
#include "ScriptGlobal.hpp"
#include "TransitionState.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandArNotifications::CommandArNotifications(CommandList* const parent)
		: CommandToggle(parent, LOC("ARNOTIFS"), CMDNAMES("arnotifs"))
	{
	}

	void CommandArNotifications::onEnable(Click& click)
	{
		if (!CommandToggleRegularSp::checkPermissions())
		{
			click.setResponse(LOC("CMDFULLSP"));
			auto click_ = click.deriveAuto();
			setStateBool(click_, false);
			return;
		}
		registerScriptTickEventHandler(click, [this]
		{
			HANDLER_CHECK(m_on)
			if (!g_tb_screenshot_mode.isEnabled()
				&& *pointers::CLoadingScreens_ms_Context == 0
				&& ScriptGlobal(GLOBAL_TRANSITION_STATE).get<int>() != TRANSITION_STATE_FM_TRANSITION_CREATE_PLAYER
				)
			{
				if (CommandToggleRegularSp::checkPermissions()
					&& scaleform.ensureLoaded()
					)
				{
					// Init scaleform
					scaleform["SET_DISPLAY_CONFIG"](1280, 720, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, false, false);
					scaleform["SET_HELPTEXT_HEIGHT"](top_offset->getFloatValue());
					scaleform["SET_MINIMAP_VISIBLE_STATE"](0); // DISPLAY_STATE_MAP_OFF
					scaleform["SHOW_CONTENT"]();
					scaleform["ENABLE_SHOW_DEBUG_BOUNDS"](debug_mode->m_on);

					// Init variables
					auto pos = g_cam_ent.getPos();

					auto ent_rot = g_cam_ent.getRot();
					ent_rot.y = 0.0f;
					v3 notif_rot;
					switch (rotation_mode->value)
					{
					default:
					case 0:
						notif_rot = ent_rot;
						notif_rot.x = 0.0f;
						ensureTextReadability(notif_rot);
						break;

					case 1:
						notif_rot = ent_rot;
						ensureTextReadability(notif_rot);
						break;

					case 2:
						notif_rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
						notif_rot.x = 0.0f;
						break;

					case 3:
						notif_rot = CAM::GET_FINAL_RENDERED_CAM_ROT(2);
						break;
					}
					if (!g_cam_veh.isValid())
					{
						notif_rot.x = 0.0f;
					}
					notif_rot.y = 0.0f;

					const bool is_first_person = (
						(CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) == 4)
						|| CAM::IS_CINEMATIC_IDLE_CAM_RENDERING()
						);

					float scalef;
					if (dynamic_scaling->m_on
						&& !is_first_person
						&& !g_tb_freecam.isEnabled()
						&& !CAM::IS_CINEMATIC_CAM_RENDERING()
						)
					{
						scalef = dynamic_scaler->getFloatValue() * (CAM::GET_FINAL_RENDERED_CAM_COORD().distance(pos));
					}
					else
					{
						scalef = static_scaler->getFloatValue();
					}

					// Apply user's vertical offset
					pos += (v3{ 90.0f, 0.0f, ent_rot.z - 90.0f }.toDir() * (is_first_person
						? fp_vertical_offset->getFloatValue()
						: vertical_offset->getFloatValue()
						));

					// Apply user's horizontal offset
					pos += (v3{ 0.0f, 0.0f, ent_rot.z - 90.0f }.toDir() * (is_first_person
						? fp_horizontal_offset->getFloatValue()
						: g_cam_ent.getDimensions().x * horizontal_offset->getFloatValue()
						));

					// Apply user's depth offset
					pos += (ent_rot.toDirNoZ() * (is_first_person
						? fp_depth_offset->getFloatValue()
						: depth_offset->getFloatValue()
						));

					// Fix notification visual position vertically
					notif_rot.x += 90.0f;
					pos += (notif_rot.toDir() * (scalef * (720.0f / 1280.0f) * 0.5f));
					notif_rot.x -= 90.0f;

					// Fix notification visual position horizontally
					pos += (v3{ 0.0f, 0.0f, notif_rot.z - 90.0f }.toDir() * ((0.5f - 0.07f) * scalef));

					if (!g_cam_veh.isValid()
						|| rotation_mode->value == 0
						|| rotation_mode->value == 2
						)
					{
						notif_rot.z *= -1.0f;
					}
					scaleform.draw3d(pos, notif_rot, scalef);
				}
			}
			else
			{
				if (scaleform)
				{
					free();
				}
			}
			HANDLER_END
		});
	}

	void CommandArNotifications::onDisable(Click& click)
	{
		ensureScriptThread(click, [this]
		{
			free();
		});
	}

	void CommandArNotifications::ensureTextReadability(v3& notif_rot) const
	{
		if (ensure_text_readability->m_on
			&& Util::angles_dist(notif_rot.z, CAM::GET_FINAL_RENDERED_CAM_ROT(2).z) > 90.0f
			)
		{
			notif_rot.z += 180.0f;
		}
	}

	void CommandArNotifications::free()
	{
		if (debug_mode->m_on)
		{
			scaleform["ENABLE_SHOW_DEBUG_BOUNDS"](false);
		}
		scaleform.drawFullscreen();
		scaleform.forget();
	}
}
