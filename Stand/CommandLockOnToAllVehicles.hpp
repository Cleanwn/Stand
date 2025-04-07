#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandLockOnToAllVehicles : public CommandToggle
	{
	private:
		std::unordered_set<Entity> entities;

	public:
		explicit CommandLockOnToAllVehicles(CommandList* const parent);

		void onEnable(Click& click) final;
		void onDisable(Click& click) final;
	};
}