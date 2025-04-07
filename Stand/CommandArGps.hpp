#pragma once

#include "CommandToggle.hpp"

#define USE_GAME_GPS true

#include "AbstractEntity.hpp"
#include "DrawUtil3d.hpp"
#include "Renderer.hpp"
#include "tbArSpinner.hpp"

#if USE_GAME_GPS
#include "CGpsSlot.hpp"
#else
#include "Exceptional.hpp"
#include "Gui.hpp"
#include "PathFind.hpp"
#endif

namespace Stand
{
	class CommandArGps : public CommandToggle
	{
	public:
#if USE_GAME_GPS
		bool enabled_ar_spinner = false;
#else
		bool recalc = false;
		v3 route_for{};
#if USE_SOUP_PATHFIND
		std::vector<PathNode*> route;
#else
		std::vector<const PathNode*> route;
#endif
#endif

		explicit CommandArGps(CommandList* const parent)
			: CommandToggle(parent, LOC("ARGPS"), { CMDNAME("argps") }, LOC("ARGPS_H"))
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]
			{
				HANDLER_CHECK(m_on);
#if USE_GAME_GPS
				if (pointers::gps_slots[0].m_NumNodes != 0 && g_player_veh.isValid())
				{
					int i = pointers::gps_slots[0].m_NumNodes - 1;
					auto pos = pointers::gps_slots[0].m_NodeCoordinates[i];
					if (pointers::gps_slots[0].isPartialRoute())
					{
						if (!enabled_ar_spinner)
						{
							enabled_ar_spinner = true;
							g_tb_ar_spinner.enable();
						}
					}
					else
					{
						if (enabled_ar_spinner)
						{
							enabled_ar_spinner = false;
							g_tb_ar_spinner.disable();
						}
					}
					do
					{
						if (*reinterpret_cast<int*>(&pointers::gps_slots[0].m_NodeCoordinates[i].w) == GNI_IGNORE_FOR_NAV)
						{
							continue; // Avoid zig-zagging due to intersections
						}
						DrawUtil3d::draw_line(pos.x, pos.y, pos.z, pointers::gps_slots[0].m_NodeCoordinates[i].x, pointers::gps_slots[0].m_NodeCoordinates[i].y, pointers::gps_slots[0].m_NodeCoordinates[i].z + 1.0f, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
						pos = pointers::gps_slots[0].m_NodeCoordinates[i];
						pos.z += 1.0f;
					} while (i-- != 0);
				}
				else
				{
					if (enabled_ar_spinner)
					{
						enabled_ar_spinner = false;
						g_tb_ar_spinner.disable();
					}
				}
#else
				bool need_recalc = false;
				v3 dest = g_gui.waypoint;
				if (dest.isNull())
				{
					if (!route.empty())
					{
						route_for.reset();
						route.clear();

						PathFind::nodes.clear(); // really shitty fix for random issues where pathfinding gives impossible routes
					}
				}
				else
				{
					if (dest != route_for)
					{
						need_recalc = true;
					}
					else if (!route.empty())
					{
						auto node = PathFind::getClosestNode(g_player_ent.getPos());
						if (std::find(route.begin(), route.end(), node) == route.end())
						{
							need_recalc = true;
						}
					}
					if (need_recalc && !recalc)
					{
						recalc = true;
						route_for = dest;
						g_tb_ar_spinner.enable();
						Exceptional::createManagedThread([this]
						{
							__try
							{
								route = PathFind::calculateRoute(g_player_ent.getPos(), route_for);
							}
							__EXCEPTIONAL()
							{
							}
							recalc = false;
							g_tb_ar_spinner.disable();
						});
					}
					if (!route.empty()
						&& !DrawUtil3d::exclusive_ar
						)
					{
						v3 pos = route.at(0)->pos;
						for (const auto& node : route)
						{
							DrawUtil3d::draw_line_native(pos.x, pos.y, pos.z, node->pos.x, node->pos.y, node->pos.z + 1.0f, (int)(g_renderer.arColour.x * 255.0f), (int)(g_renderer.arColour.y * 255.0f), (int)(g_renderer.arColour.z * 255.0f), (int)(g_renderer.arColour.w * 255.0f));
							pos = node->pos;
							pos.z += 1.0f;
						}
					}
				}
#endif
				HANDLER_END;
			});
		}

#if USE_GAME_GPS
		void onDisable(Click& click) final
		{
			if (enabled_ar_spinner)
			{
				enabled_ar_spinner = false;
				g_tb_ar_spinner.disable();
			}
		}
#endif
	};
}
