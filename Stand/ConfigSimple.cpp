#include "ConfigSimple.hpp"

#include "ColonsAndTabs.hpp"

namespace Stand
{
	void ConfigSimple::loadImpl(std::unordered_map<std::string, std::string>&& data)
	{
		this->data = std::move(data);
	}

	void ConfigSimple::saveToFileNow()
	{
		std::ofstream of(getPath());
		ColonsAndTabs::writeBySchema(of, data, getOrder());
	}

	void ConfigSimple::setDefault(const std::string& key, std::string&& value)
	{
		auto e = data.find(key);
		if (e == data.end())
		{
			data.emplace(key, std::move(value));
		}
	}
}
