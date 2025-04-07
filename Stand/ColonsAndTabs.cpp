#include "ColonsAndTabs.hpp"

#include <soup/cat.hpp>
#include <soup/catTreeReader.hpp>
#include <soup/IstreamReader.hpp>

#include "StringUtils.hpp"

#ifdef STAND_DEBUG
#include "Util.hpp"
#endif

namespace Stand
{
	void ColonsAndTabs::writeBySchemaImpl(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data, const std::vector<std::string>& write_order)
	{
		for (const auto& key : write_order)
		{
			auto entry = data.find(key);
			if (entry != data.end())
			{
				std::string name = entry->first;
				StringUtils::replace_all(name, ":", "\\:");
				file_out << name << ": " << entry->second << '\n';
			}
		}
	}

	void ColonsAndTabs::writeBySchema(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data, const std::vector<std::string>& write_order, bool schema_intentionally_incomplete)
	{
		writeBySchemaImpl(file_out, data, write_order);
#ifdef STAND_DEBUG
		if (!schema_intentionally_incomplete)
		{
			for (const auto& entry : data)
			{
				if (std::find(write_order.begin(), write_order.end(), entry.first) == write_order.end())
				{
					Util::toast(fmt::format("\"{}\" is not in schema; was discarded during ColonsAndTabs::writeBySchema.", entry.first), TOAST_ALL);
				}
			}
		}
#endif
	}

	void ColonsAndTabs::write(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data, const std::vector<std::string>& write_order)
	{
		writeBySchemaImpl(file_out, data, write_order);
		for (const auto& entry : data)
		{
			if (std::find(write_order.begin(), write_order.end(), entry.first) == write_order.end())
			{
				std::string name = entry.first;
				StringUtils::replace_all(name, ":", "\\:");
				file_out << name << ": " << entry.second << '\n';
			}
		}
	}

	void ColonsAndTabs::write(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data)
	{
		for (const auto& entry : data)
		{
			file_out << entry.first << ": " << entry.second << '\n';
		}
	}

	void ColonsAndTabs::read(std::ifstream& file_in, std::unordered_map<std::string, std::string>& data)
	{
		soup::IstreamReader r(file_in);
		if (auto root = soup::cat::parse(r))
		{
			soup::catTreeReader tr;
			data = tr.toMap(root.get(), false);
		}
		else
		{
			// Malformed data. Treat it as an empty file.
			data.clear();
		}
	}
}
