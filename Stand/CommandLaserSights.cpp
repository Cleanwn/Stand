#include "CommandLaserSights.hpp"

#include "AbstractEntity.hpp"
#include "gta_input.hpp"
#include "input.hpp"
#include "natives.hpp"
#include "Renderer.hpp"

namespace Stand
{
	CommandLaserSights::CommandLaserSights(CommandList* const parent)
		: CommandToggle(parent, LOC("LASGUN"), CMDNAMES_OBF("lasersights", "lazersights"))
	{
	}

	void CommandLaserSights::onEnable(Click& click)
	{
		registerScriptTickEventHandler([this]
		{
			HANDLER_CHECK(m_on)
			if (Input::isPressingAim())
			{
				weapon_pos = g_player_ped.getWeaponMuzzlePos();
			}
			else
			{
				weapon_pos.reset();
			}
			HANDLER_END
		});
		registerPresentEventHandler([this]
		{
			HANDLER_CHECK(m_on)
			if (!weapon_pos.isNull())
			{
				bool should_draw = false;
				float x, y;

				if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(
					weapon_pos.x,
					weapon_pos.y,
					weapon_pos.z,
					&x,
					&y
				))
				{
					should_draw = true;
				}
				else
				{
					auto pos = g_player_ped.getPos();
					if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(
						pos.x,
						pos.y,
						pos.z,
						&x,
						&y
					))
					{
						should_draw = true;
					}
				}
				if (should_draw)
				{
#if true
					g_renderer.drawLineCP(x, y, 0.5, 0.5, g_renderer.arColour);
#else
					const auto target_pos = CAM::GET_FINAL_RENDERED_CAM_COORD() + (CAM::GET_FINAL_RENDERED_CAM_ROT(2).toDir() * WEAPON::GET_MAX_RANGE_OF_CURRENT_PED_WEAPON(g_player_ped));
					float target_x, target_y;
					if (GRAPHICS::GET_SCREEN_COORD_FROM_WORLD_COORD(
						target_pos.x,
						target_pos.y,
						target_pos.z,
						&target_x,
						&target_y
					))
					{
						g_renderer.drawLineCP(x, y, target_x, target_y, g_renderer.arColour);
					}
#endif
				}
			}
			HANDLER_END
		});
	}
}
