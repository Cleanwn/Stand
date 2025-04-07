#pragma once

#include <string>

namespace Stand
{
	[[nodiscard]] extern bool validate_file_name(std::wstring name, bool allow_folder);
}
