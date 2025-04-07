#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "get_current_time_millis.hpp"
#include "vector.hpp"

// Seems to be a bit faster than the garbage I initially wrote specifically for Stand.
// Also doesn't seem to introduce any issues that weren't there before, so we shall use it.
#define USE_SOUP_PATHFIND true

namespace Stand
{
	union PathNodeIdentity
	{
		uint64_t id;
		struct
		{
			int16_t x;
			int16_t y;
			int16_t z;
			int16_t pad;
		};

		[[nodiscard]] bool operator==(const PathNodeIdentity& b) const noexcept;

		template <typename V3>
		[[nodiscard]] static PathNodeIdentity get(const V3& pos)
		{
			PathNodeIdentity id;
			id.x = int16_t(pos.x);
			id.y = int16_t(pos.y);
			id.z = int16_t(pos.z);
			id.pad = 0;
			return id;
		}
	};
	static_assert(sizeof(PathNodeIdentity) == 8);

	class PathNode
	{
	public:
		const PathNodeIdentity id;
		v3 pos;
		std::vector<float> directions;
	private:
		std::vector<PathNode*> neighbors{};

	public:
		PathNode(PathNodeIdentity id, const rage::scrVector3& pos, float heading);
	private:
		void considerDirection(const v3& pos, float heading);

	public:
		[[nodiscard]] const std::vector<PathNode*>& getNeighbors();
	private:
		void populateNeighbors();
		void populateNeighborsConsiderDirection(float heading, bool forwards, const float* offs, size_t offs_size);
	};

	struct RouteNode
	{
		PathNode* node;
		const RouteNode* prev;
	};

	struct UnvisitedRouteNode : public RouteNode
	{
		float cost;
	};

	class PathFind
	{
	public:
		inline static std::vector<std::unique_ptr<PathNode>> nodes;

		[[nodiscard]] static PathNode* getNode(const PathNodeIdentity id);
		[[nodiscard]] static PathNode* getClosestNode(const v3& pos);

#if USE_SOUP_PATHFIND
		[[nodiscard]] static std::vector<PathNode*> calculateRoute(const v3& _start, const v3& _dest);
#else
		[[nodiscard]] static std::vector<const PathNode*> calculateRoute(const v3& _start, const v3& _dest);
#endif
	private:
		static void finishRoute(std::vector<const PathNode*>& route, const RouteNode* node);

	public:
		inline static std::vector<std::pair<v3, time_t>> recent_natural_spawns;

		[[nodiscard]] static bool probablyOccupied(const v3& pos, const float range);
		[[nodiscard]] static v3 getClosestNaturalSpawnPosition(AbstractModel model, const v3& pos, float& heading, int starting_node_index = 5, bool roadside = true, float min_range = 15.0f, float max_range = 50.0f, float min_separation = 5.0f);
	};
}
