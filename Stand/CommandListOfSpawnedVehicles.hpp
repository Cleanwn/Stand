#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListOfSpawnedVehicles : public CommandList
	{
	private:
		static inline CommandList* instance = nullptr;

	public:
		explicit CommandListOfSpawnedVehicles(CommandList* const parent);

		static void makeListItem(AbstractEntity& spawned_vehicle);
	};
}