#pragma once

#include <cstdint>
#include <string>

#include <soup/Exception.hpp>

namespace Stand
{
	struct Codename
	{
		uint8_t category;
		uint8_t id;

		consteval Codename(const char* data)
		{
			if (data[0] < 'A' || data[0] > 'Z')
			{
				throw soup::Exception("Codename: Invalid category");
			}
			category = (data[0] - 'A');

			if (data[1] >= '0' && data[1] <= '9')
			{
				id = (data[1] - '0');
			}
			else if (data[1] >= 'A' && data[1] <= 'Z')
			{
				id = (data[1] - 'A') + 10;
			}
			else
			{
				throw soup::Exception("Codename: Invalid id");
			}
			
			if (data[2] != '\0')
			{
				throw soup::Exception("Codename: String too long");
			}
		}

		void operator=(const char*) = delete;
		void operator=(const std::string&) = delete;

		[[nodiscard]] std::string toString() const noexcept
		{
			std::string str;
			str.reserve(2);
			str.push_back(category + 'A');
			if (id < 10)
			{
				str.push_back(id + '0');
			}
			else
			{
				str.push_back((id - 10) + 'A');
			}
			return str;
		}
	};
}
