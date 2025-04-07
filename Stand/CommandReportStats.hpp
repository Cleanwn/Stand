#pragma once

#include "CommandListReadonly.hpp"

namespace Stand
{
	class CommandReportStats : public CommandListReadonly
	{
	public:
		explicit CommandReportStats(CommandList* const parent);

		void onClick(Click& click) final;
	};
}
