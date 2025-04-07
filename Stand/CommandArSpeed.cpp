#include "CommandArSpeed.hpp"

#include "AbstractEntity.hpp"
#include "CommandSlider.hpp"
#include "natives.hpp"
#include "Renderer.hpp"
#include "Scaleform.hpp"
#include "tbScreenshotMode.hpp"
#include "Util.hpp"

namespace Stand
{
	CommandArSpeed::CommandArSpeed(CommandList* const parent, CommandColour* const colour, CommandToggleNoCorrelation* const alt_rendering)
		: CommandToggle(parent, LOC("ARSPD"), CMDNAMES("arspeedometer", "arspeed"), LOC("ARSPD_H")), colour(colour), alt_rendering(alt_rendering)
	{
	}

	static ScaleformTextcanvas textcanvas_scaleform{};

	void CommandArSpeed::onEnable(Click& click)
	{
		ThreadContext thread_context = click.thread_context;
		if (click.type == CLICK_BULK)
		{
			thread_context = TC_OTHER;
		}
		registerScriptTickEventHandler(thread_context, [this]()
		{
			if (!m_on)
			{
				textcanvas_scaleform.unload();
				return false;
			}
			if (!HUD::IS_PAUSE_MENU_ACTIVE()
				&& !g_tb_screenshot_mode.isEnabled()
				)
			{
				auto veh = g_player_ped.getVehicle(false);
				float text_scale = 1.0f;
				if (veh.isValid())
				{
					if (CAM::GET_FOLLOW_VEHICLE_CAM_VIEW_MODE() != 4)
					{
						text_scale = std::min(veh.getDimensions().y, 10.0f);
					}
				}
				else if (CAM::IS_AIM_CAM_ACTIVE())
				{
					return true;
				}
				v3 pos = g_player_ped.getHeadPos();
				pos = pos + (ENTITY::GET_ENTITY_FORWARD_VECTOR(g_player_ped) * text_scale);
				if (alt_rendering->m_on)
				{
					if (textcanvas_scaleform.ensureLoaded())
					{
						textcanvas_scaleform.setText(Util::mps_to_unit_of_choice(g_player_ent.getSpeed()).c_str());
						textcanvas_scaleform.setColour(Util::get_closest_hud_colour(colour->r->value, colour->g->value, colour->b->value, colour->a->value));
						textcanvas_scaleform.draw3d(pos, CAM::GET_FINAL_RENDERED_CAM_ROT(2), text_scale * 2.2f);
					}
				}
				else
				{
					g_renderer.draw3dTextThisTickH(pos, Util::mps_to_unit_of_choice_w(g_player_ent.getSpeed()), text_scale, colour->getRGBA());
				}
			}
			return true;
		});
	}
}
