#pragma once

#include "CommandSliderFloat.hpp"

#include "CommandExtraInfo.hpp"
#include "hooking.hpp"
#include "pointers.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandVisualSetting : public CommandSliderFloat
	{
	public:
		CommandVisualSetting(CommandList* const parent, std::string name, float vanilla_value)
			: CommandSliderFloat(parent, LIT(std::move(name)), {}, NOLABEL, LONG_MIN, LONG_MAX, (int)(vanilla_value * 100.0f), 50)
		{
			std::string fqn = menu_name.literal_str;
			for (CommandList* list = parent; list->type != COMMAND_LIST_CUSTOM_SPECIAL_MEANING; list = list->parent)
			{
				fqn.insert(0, 1, '.');
				fqn.insert(0, list->menu_name.literal_str);
			}

			help_text.setLiteral(fqn);

			StringUtils::simplify(fqn);
			fqn.insert(0, "vis");
			command_names = { utf8ToCmdName(fqn) };
		}

		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final
		{
			info.completed_hint = LANG_GET_W("VISSTTNGS");
			info.completed_hint.append(L": ");
			info.completed_hint.append(help_text.getLocalisedUtf16());
		}

		void onChange(Click& click, int prev_value) final
		{
			if (value != default_value)
			{
				g_hooking.visual_config_overrides[help_text.getLiteralUtf8()] = getFloatValue();
			}
			else
			{
				g_hooking.visual_config_overrides.erase(help_text.getLiteralUtf8());
			}
			pointers::CVisualSettings_LoadAll();
		}
	};
}
