#pragma once

#include "CommandAction.hpp"

#include <fmt/format.h>

#include "lang.hpp"

namespace Stand
{
	class CommandGamerPreset : public CommandAction
	{
	private:
		const std::pair<int64_t, const char*>* const data;

	public:
		explicit CommandGamerPreset(CommandList* const parent, const std::pair<int64_t, const char*>* const data)
			: CommandAction(parent, LIT(data->second), {}, LIT(std::move(std::string("Rockstar ID: ").append(fmt::to_string(data->first)))), CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST), data(data)
		{
		}

		explicit CommandGamerPreset(CommandList* const parent, const char* menu_name, const std::pair<int64_t, const char*>* const data)
			: CommandAction(parent, LIT(menu_name), {}, LIT(std::move(std::string(LANG_GET("NME")).append(": ").append(data->second).append("\nRockstar ID: ").append(fmt::to_string(data->first)))), CMDFLAGS_ACTION | CMDFLAG_SEARCH_FINISHLIST | CMDFLAG_FEATURELIST_FINISHLIST), data(data)
		{
		}

		void onClick(Click& click) final
		{
			parent->parent->as<CommandGamerPresets>()->onPresetClick(click, data->first, data->second);
		}
	};
}
