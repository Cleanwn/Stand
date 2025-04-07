#include "PathFind.hpp"

#include "AbstractEntity.hpp"
#include "AbstractModel.hpp"
#include "natives.hpp"
#include "Util.hpp"

#if USE_SOUP_PATHFIND
#include <soup/Pathfind.hpp>
#endif

namespace Stand
{
	bool PathNodeIdentity::operator==(const PathNodeIdentity& b) const noexcept
	{
		return id == b.id;
	}

	PathNode::PathNode(PathNodeIdentity id, const rage::scrVector3& pos, float heading)
		: id(id), pos(pos), directions(1, heading)
	{
		for (float off = 0.0f; off <= 270.0f; off += 90.0f)
		{
			considerDirection(this->pos + v3(0.0f, 0.0f, heading + off).toDirNoZ(), heading);
		}
		if (directions.size() == 1)
		{
			for (float off = 0.0f; off <= 270.0f; off += 90.0f)
			{
				considerDirection(this->pos + (v3(0.0f, 0.0f, heading + off).toDirNoZ() * 5.0f), heading);
			}
			/*if (directions.size() == 1)
			{
				considerDirection(this->pos + (v3(0.0f, 0.0f, heading + 180.0f).toDirNoZ() * 10.0f), heading);
			}*/
		}
	}

	void PathNode::considerDirection(const v3& pos, float heading)
	{
		Vector3 outPos;
		float outHeading;
		PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, &outPos, &outHeading, 1, 1.0f, 0);
		while (outHeading > 360.0f)
		{
			outHeading -= 360.0f;
		}
		for (const auto& dir : directions)
		{
			if (Util::angles_dist(dir, outHeading) < 15.0f)
			{
				return;
			}
		}
		directions.emplace_back(outHeading);
	}

	const std::vector<PathNode*>& PathNode::getNeighbors()
	{
		if (neighbors.empty())
		{
			populateNeighbors();
		}
		return neighbors;
	}

	static constexpr float offs_forwards[] = {
		5.0f,
		7.0f,
		10.0f,
		20.0f,
		30.0f, // The fucking airport
	};

	static constexpr float offs_side[] = {
		5.0f,
		//10.0f,
		15.0f,
	};

	void PathNode::populateNeighbors()
	{
		uint8_t dir_i = 0;
		for (auto i = directions.begin(); i != directions.end(); ++i, ++dir_i)
		{
			if (dir_i == 0)
			{
				populateNeighborsConsiderDirection(*i, true, offs_forwards, COUNT(offs_forwards));
			}
			else
			{
				populateNeighborsConsiderDirection(*i, false, offs_side, COUNT(offs_side));
			}
		}
	}

	void PathNode::populateNeighborsConsiderDirection(float heading, bool forwards, const float* offs, size_t offs_size)
	{
		PathNode* found = nullptr;
		uint8_t off_i = 0;
		for (; off_i < offs_size; ++offs, ++off_i)
		{
			auto neighbor = PathFind::getClosestNode(pos + (v3(0.0f, 0.0f, heading).toDirNoZ() * (*offs)));
			if (neighbor == this
				|| neighbor == found
				//|| abs(pos.z - neighbor->pos.z) > 5.0f
				)
			{
				continue;
			}
			neighbors.emplace_back(neighbor);
			if (!forwards || neighbor->directions.size() > 1)
			{
				break;
			}
			found = neighbor;
		}
	}

	PathNode* PathFind::getNode(const PathNodeIdentity id)
	{
		for (const auto& node : nodes)
		{
			if (node->id == id)
			{
				return node.get();
			}
		}
		return nullptr;
	}

	PathNode* PathFind::getClosestNode(const v3& pos)
	{
		Vector3 outPos;
		float outHeading;
		PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, &outPos, &outHeading, 1, 1.0f, 0);
		auto id = PathNodeIdentity::get(outPos);
		PathNode* node = getNode(id);
		if (node == nullptr)
		{
			node = nodes.emplace_back(std::make_unique<PathNode>(id, outPos, outHeading)).get();
		}
		return node;
	}

#if USE_SOUP_PATHFIND
	struct PathfindQueryInterface
	{
		static uint64_t getUniqueId(PathNode* node)
		{
			return node->id.id;
		}

		static float getDistance(PathNode* a, PathNode* b)
		{
			return a->pos.distance(b->pos);
		}

		static float getHeuristicScore(PathNode* node, PathNode* end)
		{
			return getDistance(node, end);
		}

		static std::vector<PathNode*> getNeighbours(PathNode* node)
		{
			return node->getNeighbors();
		}
	};
#endif

#if USE_SOUP_PATHFIND
	std::vector<PathNode*> PathFind::calculateRoute(const v3& _start, const v3& _dest)
#else
	std::vector<const PathNode*> PathFind::calculateRoute(const v3& _start, const v3& _dest)
#endif
	{
		PathNode* const start = getClosestNode(_start);
		PathNode* const dest = getClosestNode(_dest);
#if USE_SOUP_PATHFIND
		return soup::Pathfind<PathfindQueryInterface, PathNode*>::route(start, dest).second;
#else
		if (start == dest)
		{
			return std::vector<const PathNode*>(1, start);
		}
		std::vector<std::unique_ptr<RouteNode>> visited{};
		std::vector<UnvisitedRouteNode> unvisited = { UnvisitedRouteNode{ start, nullptr, 0.0f } };
		while (!unvisited.empty())
		{
			const auto base_cost = unvisited.cbegin()->cost;
			RouteNode* node = visited.emplace_back(std::make_unique<RouteNode>(std::move(*unvisited.begin()))).get();
			unvisited.erase(unvisited.begin());
			for (const auto& neighbor : node->node->getNeighbors())
			{
				if (neighbor == dest)
				{
					std::vector<const PathNode*> route = { dest };
					finishRoute(route, node);
					return route;
				}
				bool known = false;
				for (const auto& un : visited)
				{
					if (un->node == neighbor)
					{
						known = true;
						break;
					}
				}
				if (!known)
				{
					for (const auto& un : unvisited)
					{
						if (un.node == neighbor)
						{
							known = true;
							break;
						}
					}
					if (!known)
					{
						auto cost = (base_cost + neighbor->pos.distance(node->node->pos));
						// Optimised sorted emplace
						auto i = unvisited.begin();
						for (; i != unvisited.end(); ++i)
						{
							if (i->cost > cost)
							{
								break;
							}
						}
						if (i == unvisited.end())
						{
							unvisited.emplace_back(UnvisitedRouteNode{ neighbor, node, cost });
						}
						else
						{
							unvisited.emplace(i, UnvisitedRouteNode{ neighbor, node, cost });
						}
					}
				}
			}
		}
		// Destination unreachable; optimised sorted at(0)
		const RouteNode* winner = nullptr;
		float winner_dist = FLT_MAX;
		for (auto i = visited.cbegin(); i != visited.cend(); ++i)
		{
			auto dist = (*i)->node->pos.distance(dest->pos);
			if (dist < winner_dist)
			{
				winner = i->get();
				winner_dist = dist;
			}
		}
		std::vector<const PathNode*> route = {};
		finishRoute(route, winner);
		return route;
#endif
	}

	void PathFind::finishRoute(std::vector<const PathNode*>& route, const RouteNode* node)
	{
		do
		{
			route.emplace_back(node->node);
			node = node->prev;
		} while (node != nullptr);
	}

	bool PathFind::probablyOccupied(const v3& pos, const float range)
	{
		for (auto i = recent_natural_spawns.begin(); i != recent_natural_spawns.end(); )
		{
			if (GET_MILLIS_SINCE(i->second) > 10000)
			{
				i = recent_natural_spawns.erase(i);
			}
			else
			{
				if (i->first.distance(pos) < range)
				{
					return true;
				}

				++i;
			}
		}

		if (MISC::IS_POSITION_OCCUPIED(pos.x, pos.y, pos.z, range, FALSE, TRUE, TRUE, FALSE, FALSE, 0, FALSE))
		{
			return true;
		}

		return false;
	}

	v3 PathFind::getClosestNaturalSpawnPosition(AbstractModel model, const v3& pos, float& heading, int starting_node_index, bool roadside, float min_range, float max_range, float min_separation)
	{
		int lane;
		int node_type = roadside ? 0 : 1; // If roadside, then prefer major roads.
		Vector3 result = Vector3(pos);

		if (model.isBoat())
		{
			if (ENTITY::IS_ENTITY_IN_WATER(g_player_ped)) // Pathfinding nodes will always return the closest point of water, which will be on top of us.
			{
				float unused;
				uint8_t attempts = 0;

				while (++attempts < 255)
				{
					auto x = (float)soup::rand(10, 50);
					auto y = (float)soup::rand(10, 50);
					result.x += soup::rand.coinflip() ? x : -x;
					result.y += soup::rand.coinflip() ? y : -y;

					if (WATER::GET_WATER_HEIGHT(pos.x, pos.y, pos.z, &unused))
					{
						break;
					}
				}

				return result;
			}
			else
			{
				node_type = 3; // Water
			}
		}

		int node_index = model.isBoat() ? 1 : starting_node_index;
		int backup_node_index = model.isBoat() ? node_type : 1; // Backup node index is only relevant for land vehicles, as we prefer major routes.
		do
		{
			if (!PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, node_index, &result, &heading, &lane, node_type, 3.0f, 0.0f) || result.distance(pos) > max_range)
			{
				PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_WITH_HEADING(pos.x, pos.y, pos.z, node_index, &result, &heading, &lane, backup_node_index, 3.0f, 0.0f); // If we can't find anything, then go with any viable path.
			}

			if (++node_index == 255)
			{
				break;
			}
		}
		while (probablyOccupied(result, min_separation) || result.distance(pos) < min_range);

		if (!result.isNull())
		{
			if (roadside
				&& !model.isAirborne()
				&& !model.isAquatic()
				)
			{
				PATHFIND::GET_ROAD_BOUNDARY_USING_HEADING(result.x, result.y, result.z, heading, &result);
			}

			recent_natural_spawns.emplace_back(std::make_pair(result, get_current_time_millis()));

			return result;
		}

		return pos;
	}
}
