#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"
#include "PathFind.hpp"

namespace Stand
{
	class CommandShowNavMeshNeighbors : public CommandToggle
	{
	public:
		explicit CommandShowNavMeshNeighbors(CommandList* const parent)
			: CommandToggle(parent, LIT("Show Nav Mesh Neighbors"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
				auto node = PathFind::getClosestNode(g_player_ent.getPos());
				auto neighbors = node->getNeighbors();
				if (neighbors.empty())
				{
					DrawUtil3d::draw_line(node->pos.x, node->pos.y, node->pos.z, node->pos.x, node->pos.y, node->pos.z + 1.0f, 255, 0, 0, 255);
				}
				else
				{
					for (const auto& neighbor : neighbors)
					{
						DrawUtil3d::draw_line(node->pos.x, node->pos.y, node->pos.z, neighbor->pos.x, neighbor->pos.y, neighbor->pos.z + 1.0f, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
					}
				}
				HANDLER_END;
			});
		}
	};
}
