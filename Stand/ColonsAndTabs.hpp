#pragma once

#include <fstream>
#include <unordered_map>

namespace Stand
{
	class ColonsAndTabs
	{
	private:
		static void writeBySchemaImpl(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data, const std::vector<std::string>& write_order);
	public:
		static void writeBySchema(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data, const std::vector<std::string>& write_order, bool schema_intentionally_incomplete = false);
		static void write(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data, const std::vector<std::string>& write_order);
		static void write(std::ofstream& file_out, const std::unordered_map<std::string, std::string>& data);

		static void read(std::ifstream& file_in, std::unordered_map<std::string, std::string>& data);
	};
}
