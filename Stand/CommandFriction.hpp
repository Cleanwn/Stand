#pragma once

#include "CommandToggle.hpp"

namespace Stand
{
	class CommandFriction : public CommandToggle
	{
	private:
		bool reset_vel = false;

	public:
		explicit CommandFriction(CommandList* const parent);

		void onEnable(Click& click) final;
	};
}
