#pragma once

#include <functional>
#include <optional>
#include <stack>

#include "fwddecl.hpp"
#include "nullsub.hpp"
#include "PositionState.hpp"

namespace Stand
{
	struct TpUtil
	{
		inline static std::optional<v3> last_tp{};
		inline static time_t last_tp_time{};
		inline static std::stack<PositionState> undo_stack{};

		[[nodiscard]] static Vector2Plus followRedirects(const Vector2Plus& coords, bool in_vehicle);
		static void teleportWithRedirects(Vector2Plus coords, std::function<void()>&& callback = nullsub<>);
		static void teleport(Vector2Plus coords, std::function<void()>&& callback = nullsub<>);
		static void teleport_exact(v3 coords);
		static void undo_teleport();
		static void teleport_to_blip(const Blip blip);
		static void teleport_to_veh(const Vehicle veh);

		static void onPreTp(const v3&& pos);
		static void onPostTp(const v3& pos);
	};
}
