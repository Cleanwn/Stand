#pragma once

#include "ConfigTree.hpp"

namespace Stand
{
	class ConfigTreeSimple : public ConfigTree
	{
	public:
		std::unordered_map<std::string, std::string> data{};

		using ConfigTree::ConfigTree;

	protected:
		void loadImplTree(std::unordered_map<std::string, std::string>&& data) final;

		[[nodiscard]] bool getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const override;

	public:
		void reset();
	};
}
