#pragma once

#include <string>

namespace Stand
{
	[[nodiscard]] extern std::string filter_name(const std::string& name);
	[[nodiscard]] extern bool does_name_have_colour_prefix(const std::string& name);
	[[nodiscard]] extern std::string filter_name_pretty(const std::string& name);
	[[nodiscard]] extern bool is_name_length_valid(const std::string& name);
	[[nodiscard]] extern bool is_name_valid(const std::string& name);
}
