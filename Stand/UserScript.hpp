#pragma once

#include <string>
#include <vector>

namespace Stand
{
	struct UserScript
	{
		std::string description{};
		std::vector<std::string> commands{};
	};
}
