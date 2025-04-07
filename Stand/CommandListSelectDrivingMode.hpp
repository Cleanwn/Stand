#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandListSelectDrivingMode : public CommandListSelect
	{
	public:
		explicit CommandListSelectDrivingMode(CommandList* const parent);

		void onChange(Click& click, long long prev_value) final;
	};
}