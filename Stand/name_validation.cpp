#include "name_validation.hpp"

namespace Stand
{
    std::string filter_name(const std::string& name)
    {
		std::string filtered_name;
		for (const char& c : name)
		{
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.')
			{
				filtered_name.push_back(c);
			}
		}
		return filtered_name;
    }

	bool does_name_have_colour_prefix(const std::string& name)
	{
		constexpr const char* colour_prefices[] = {
			"~r~",
			"~b~",
			"~g~",
			"~y~",
			"~p~",
			"~q~",
			"~o~",
			"~c~",
			"~m~",
			"~u~",
			"~s~",
			"~w~",
			"~h~",
		};
		for (const auto& prefix : colour_prefices)
		{
			if (name.substr(0, 3) == prefix)
			{
				return true;
			}
		}
		return false;
	}

	std::string filter_name_pretty(const std::string& name)
	{
		std::string filtered_name{};

		if (does_name_have_colour_prefix(name) && !does_name_have_colour_prefix(name.substr(3)))
		{
			filtered_name = filter_name(name.substr(3));
		}
		else
		{
			filtered_name = filter_name(name);
		}

		if (filtered_name.empty())
		{
			if (name.empty())
			{
				filtered_name = "Empty_Name";
			}
			else
			{
				filtered_name = "No_Valid_Chars";
			}
		}

		return filtered_name;
	}

	bool is_name_length_valid(const std::string& name)
	{
		return name.length() >= 6 && name.length() <= 16;
	}

	bool is_name_valid(const std::string& name)
	{
		return filter_name(name) == name && is_name_length_valid(name);
	}
}
