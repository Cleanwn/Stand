#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandPackages : public CommandList
	{
	public:
		explicit CommandPackages(CommandList* parent);

		void onBack(ThreadContext thread_context) final;
	};
}
