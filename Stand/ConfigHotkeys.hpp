#pragma once

#include "ConfigTree.hpp"

namespace Stand
{
	class ConfigHotkeys : public ConfigTree
	{
	public:
		std::unordered_map<std::string, std::vector<Hotkey>> data = {};

		explicit ConfigHotkeys();

	protected:
		void loadImplTree(std::unordered_map<std::string, std::string>&& data) final;
		[[nodiscard]] bool getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const final;
	};
}
