#pragma once

#include "CommandAction.hpp"

namespace Stand
{
	class CommandRejoin : public CommandAction
	{
	public:
		explicit CommandRejoin(CommandList* parent);

		void onClick(Click& click) final;
	};
}
