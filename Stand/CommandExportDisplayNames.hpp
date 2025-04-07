#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandExportDisplayNames : public CommandAction
	{
	public:
		explicit CommandExportDisplayNames(CommandList* parent);

		void onClick(Click& click) final;
	};
}
