#pragma once

#include "AbstractEntity.hpp"

namespace Stand
{
	struct GhostDriver
	{
		inline static bool user_is_rcing = false;

		AbstractEntity driver{};

		[[nodiscard]] bool initVehicle(AbstractEntity& veh);
		void initDriver(AbstractEntity& outDriver);
		[[nodiscard]] bool needsCleanup() const noexcept;
		void cleanup();
	};
}
