#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandCtxHotkeys : public CommandList
	{
	public:
		bool close_context_menu_on_back = false;

		explicit CommandCtxHotkeys(CommandList* const parent);

		void onBack(ThreadContext thread_context) final;

	private:
		void populate();
	public:
		void update();
	};
}
