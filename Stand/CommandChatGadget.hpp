#pragma once

#include "CommandCanvas.hpp"

namespace Stand
{
	class CommandChatGadget : public CommandCanvas
	{
	public:
		explicit CommandChatGadget(CommandList* const parent);

	protected:
		void draw(soup::RenderTarget& rt) final;
	};
}
