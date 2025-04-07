#pragma once

#include "Config.hpp"

namespace Stand
{
	class ConfigSimple : public Config
	{
	public:
		std::unordered_map<std::string, std::string> data{};

	protected:
		using Config::Config;

		void loadImpl(std::unordered_map<std::string, std::string>&& data) final;

		void saveToFileNow() final;
		[[nodiscard]] virtual std::vector<std::string> getOrder() const noexcept = 0;

	public:
		void setDefault(const std::string& key, std::string&& value);
	};
}
