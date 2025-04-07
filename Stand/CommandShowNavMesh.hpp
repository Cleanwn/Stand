#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"
#include "PathFind.hpp"

namespace Stand
{
	class CommandShowNavMesh : public CommandToggle
	{
	public:
		explicit CommandShowNavMesh(CommandList* const parent)
			: CommandToggle(parent, LIT("Show Nav Mesh"), { CMDNAME("navmesh") })
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
				/*auto pos = g_player_ent.get_pos();
				for (auto x = pos.x - 50.f; x < pos.x + 50.f; x += 1.f)
				{
					for (auto y = pos.y - 50.f; y < pos.y + 50.f; y += 1.f)
					{
						Vector3 outPos;
						float outHeading;
						PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(x, y, pos.z, &outPos, &outHeading, 1, 1.0f, 0);
						auto heading_pos = outPos + v3(0.0f, 0.0f, outHeading).toDirNoZ();
						DrawUtil3d::draw_line(outPos.x, outPos.y, outPos.z, heading_pos.x, heading_pos.y, heading_pos.z + 1.0f, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
					}
				}*/
				auto node = PathFind::getClosestNode(g_player_ent.getPos());
				for (const auto& dir : node->directions)
				{
					auto heading_pos = node->pos + v3(0.0f, 0.0f, dir).toDirNoZ();
					DrawUtil3d::draw_line(node->pos.x, node->pos.y, node->pos.z, heading_pos.x, heading_pos.y, heading_pos.z + 1.0f, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
				}
				HANDLER_END;
			});
		}
	};
}
