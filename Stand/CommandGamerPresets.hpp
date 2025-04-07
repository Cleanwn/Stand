#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandGamerPresets : public CommandList
	{
	protected:
		explicit CommandGamerPresets(CommandList* const parent, Label&& menu_name, bool include_rockstar_admins);

	public:
		virtual void onPresetClick(Click& click, int64_t rid, const char* name) = 0;
	};
}
