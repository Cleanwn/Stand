#include "CommandListVisualSettings.hpp"

#include <soup/string.hpp>

#include "CommandVisualSetting.hpp"
#include "visual_settings.hpp"

#include "FileLogger.hpp"

namespace Stand
{
	static void populateSubtree(CommandList* list, std::map<std::string, float>::const_iterator& it, const std::string& prefix)
	{
		do
		{
			list->createChild<CommandVisualSetting>(it->first.substr(prefix.size()), it->second);
		} while (++it != visual_settings.cend() && prefix == it->first.substr(0, prefix.size()));
	}

	CommandListVisualSettings::CommandListVisualSettings(CommandList* const parent)
		: CommandList(parent, LOC("VISSTTNGS"), CMDNAMES("vis"), NOLABEL, CMDFLAGS_LIST, COMMAND_LIST_CUSTOM_SPECIAL_MEANING)
	{
		CommandIssuable::collapse_command_names.emplace(L"vis");

		CommandList* list = nullptr;
		for (auto it = visual_settings.cbegin(); it != visual_settings.cend();)
		{
			std::string name = it->first;
			soup::string::limit<std::string>(name, ".");
			if (name == "car"
				|| name == "heli"
				|| name == "misc"
				|| name == "ped"
				|| name == "plane"
				)
			{
				if (list == nullptr)
				{
					list = createChild<CommandList>(LIT(name));
				}

				if (auto off = it->first.find(".", name.size() + 1); off != std::string::npos)
				{
					auto prefix = it->first.substr(0, off + 1);
					populateSubtree(list->createChild<CommandList>(LIT(prefix.substr(name.size() + 1, prefix.size() - name.size() - 2))), it, prefix);
				}
				else
				{
					list->createChild<CommandVisualSetting>(it->first.substr(name.size() + 1), it->second);
					++it;
				}
			}
			else 
			{
				list = nullptr;
				if (name == "UI"
					|| name == "cam"
					|| name == "procObjects"
					)
				{
					createChild<CommandVisualSetting>(it->first, it->second);
					++it;
				}
				else
				{
					std::string prefix = name;
					prefix.push_back('.');
					populateSubtree(createChild<CommandList>(LIT(std::move(name)), {}, NOLABEL, CMDFLAG_FEATURELIST_FINISHLIST), it, prefix);
				}
			}
		}
	}
}
