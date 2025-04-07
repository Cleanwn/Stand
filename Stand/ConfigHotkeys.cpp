#include "ConfigHotkeys.hpp"

#include "CommandPhysical.hpp"
#include "extract_delimited_string_with_escaping.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	ConfigHotkeys::ConfigHotkeys()
		: ConfigTree(L"Hotkeys")
	{
	}

	void ConfigHotkeys::loadImplTree(std::unordered_map<std::string, std::string>&& data)
	{
		this->data.clear();
		for (auto& entry : data)
		{
			StringUtils::replace_all(entry.second, ", ", ",");
			std::vector<Hotkey> hotkeys = {};
			while (!entry.second.empty())
			{
				hotkeys.emplace_back(extract_delimited_string_with_escaping(entry.second, ','));
			}
			this->data.emplace(std::move(entry.first), std::move(hotkeys));
		}
	}

	bool ConfigHotkeys::getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const
	{
		if (!command->isInDefaultHotkeyState())
		{
			std::string str{};
			for (const auto& hotkey : command->hotkeys)
			{
				StringUtils::list_append_safe(str, hotkey.toFileString());
			}
			outValue = std::move(str);
			return true;
		}
		return false;
	}
}
