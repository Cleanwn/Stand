#include "parse_coord.hpp"

#include <soup/Regex.hpp>

namespace Stand
{
	std::optional<Vector2Plus> parse_coord(const std::string& str)
	{
		const soup::Regex regexp(R"((-?[0-9]+(\.[0-9]+|))f?, ?(-?[0-9]+(\.[0-9]+|))f?(, ?(-?[0-9]+(\.[0-9]+|))f?)?)");
		if (auto res = regexp.search(str); res.isSuccess())
		{
			if (res.findGroupByIndex(5))
			{
				return Vector2Plus(
					std::stof(res.findGroupByIndex(1)->toString()),
					std::stof(res.findGroupByIndex(3)->toString()),
					std::stof(res.findGroupByIndex(6)->toString())
				);
			}
			else
			{
				return Vector2Plus(
					std::stof(res.findGroupByIndex(1)->toString()),
					std::stof(res.findGroupByIndex(3)->toString())
				);
			}
		}
		return std::nullopt;
	}
}
