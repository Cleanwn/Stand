#pragma once

#include "CommandToggleRegular.hpp"

namespace Stand
{
	class CommandChatSing : public CommandToggleRegular
	{
	public:
		explicit CommandChatSing(CommandList* parent);

		void onEnableInner(Click& click) final;
		void onDisableInner(Click& click) final;
	};
}
