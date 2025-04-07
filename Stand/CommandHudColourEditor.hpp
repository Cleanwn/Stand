#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandHudColourEditor : public CommandList
	{
	public:
		explicit CommandHudColourEditor(CommandList* const parent);

	private:
		void createColourChild(const char* name, int id);
	};
}
