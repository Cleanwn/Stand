#include "tbArSpinner.hpp"

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"
#include "Renderer.hpp"

namespace Stand
{
	void tbArSpinner::onTick()
	{
		auto player_pos = g_player_ent.getPos();
		auto dim = g_player_ent.getDimensions();
		auto player_size = (dim.x + dim.y) / 2.0f;
		v3 last_pos;
		for (float i = line_i; i > line_i - 90.0f; i -= 1.0f)
		{
			auto pos = player_pos + (v3(0.0f, 0.0f, i).toDirNoZ() * player_size);
			if (i != line_i)
			{
				DrawUtil3d::draw_line(pos.x, pos.y, pos.z, last_pos.x, last_pos.y, last_pos.z, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
			}
			last_pos = pos;
		}
		if (CAM::GET_CAM_VIEW_MODE_FOR_CONTEXT(CAM::GET_CAM_ACTIVE_VIEW_MODE_CONTEXT()) == 4)
		{
			for (float i = line_i - 180.0f; i > line_i - 260.0f; i -= 1.0f)
			{
				auto pos = player_pos + (v3(0.0f, 0.0f, i).toDirNoZ() * player_size);
				if (i != line_i - 180.0f)
				{
					DrawUtil3d::draw_line(pos.x, pos.y, pos.z, last_pos.x, last_pos.y, last_pos.z, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
				}
				last_pos = pos;
			}
		}
		line_i -= 3.0f;
		if (line_i <= 0.0f)
		{
			line_i = 360.0f;
		}
	}
}
