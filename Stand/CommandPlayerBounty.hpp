#pragma once

#include "CommandPlayerActionValue.hpp"

namespace Stand
{
	class CommandPlayerBounty : public CommandPlayerActionValue
	{
	private:
		CommandToggleNoCorrelation* const bounty_self;

	public:
		explicit CommandPlayerBounty(CommandList* const parent, CommandToggleNoCorrelation* bounty_self);

	protected:
		void onCommandWithValue(Click& click, long long value) final;
	};
}
