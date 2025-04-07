#include "OnlineVersion.hpp"

#include <fmt/format.h>

#include "Label.hpp"

namespace Stand
{
	std::string OnlineVersion::getVersionString() const
	{
		std::string str = "1.";
		str.append(fmt::to_string(id / 10));
		if ((id % 10) != 0)
		{
			str.push_back('.');
			str.append(fmt::to_string(id % 10));
		}
		return str;
	}

	Label OnlineVersion::getName() const
	{
		return LOC_RT(fmt::format("DLC_{}", id));
	}
}
