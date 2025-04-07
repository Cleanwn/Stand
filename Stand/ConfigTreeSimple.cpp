#include "ConfigTreeSimple.hpp"

namespace Stand
{
	void ConfigTreeSimple::loadImplTree(std::unordered_map<std::string, std::string>&& data)
	{
		this->data = std::move(data);
	}

	bool ConfigTreeSimple::getCommandValue(const CommandPhysical* command, const std::string& path, std::string& outValue) const
	{
		const auto i = data.find(path);
		if (i != data.end())
		{
			outValue = i->second;
			return true;
		}
		return false;
	}

	void ConfigTreeSimple::reset()
	{
		ConfigTree::reset();
		data.clear();
	}
}
