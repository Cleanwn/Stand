#pragma once

#include "CommandPlayerToggle.hpp"

namespace Stand
{
	class CommandPlayerToggleBatch : public CommandPlayerToggle
	{
	protected:
		using CommandPlayerToggle::CommandPlayerToggle;

		void onEnable(Click& click) override;

		virtual void onTick(const std::vector<AbstractPlayer>& players) const = 0;
	};
}
