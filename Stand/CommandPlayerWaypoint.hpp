#pragma once

#include "CommandOnPlayer.hpp"
#include "CommandToggle.hpp"

namespace Stand
{
	class CommandPlayerWaypoint : public CommandOnPlayer<CommandToggle>
	{
	private:
		inline static bool dedup = false;

	public:
		explicit CommandPlayerWaypoint(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
