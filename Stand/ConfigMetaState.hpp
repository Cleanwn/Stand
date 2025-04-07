#pragma once

#include "Config.hpp"

namespace Stand
{
	class ConfigMetaState : public Config
	{
	public:
		std::unordered_map<std::string, std::string> data{};

		ConfigMetaState();

	protected:
		void loadImpl(std::unordered_map<std::string, std::string>&& data) final;

		void saveToFileNow() final;
	};
}
