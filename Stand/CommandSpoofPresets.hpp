#pragma once

#include "CommandGamerPresets.hpp"

namespace Stand
{
	class CommandSpoofPresets : public CommandGamerPresets
	{
	public:
		explicit CommandSpoofPresets(CommandList* parent)
			: CommandGamerPresets(parent, LOC("SPFPRST"), true)
		{
		}

		void onPresetClick(Click& click, int64_t rid, const char* name) final
		{
			CommandSpoofedName::instance->setState(name, CLICK_AUTO);
		}
	};
}
