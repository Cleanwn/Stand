#pragma once

#include <optional>
#include "Vector2Plus.hpp"

namespace Stand
{
	[[nodiscard]] extern std::optional<Vector2Plus> parse_coord(const std::string& str);
}
