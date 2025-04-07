#include "TimeOfDay.hpp"

#include "natives.hpp"
#include "str2int.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	TimeOfDay TimeOfDay::current() noexcept
	{
		TimeOfDay tod{};
		tod.hour = CLOCK::GET_CLOCK_HOURS();
		tod.minute = CLOCK::GET_CLOCK_MINUTES();
		tod.second = CLOCK::GET_CLOCK_SECONDS();
		return tod;
	}

	bool TimeOfDay::isValid() const noexcept
	{
		return hour >= 0 && hour <= 23
			&& minute >= 0 && minute <= 59
			&& second >= 0 && second <= 59;
	}

	void TimeOfDay::invalidate() noexcept
	{
		hour = -1;
	}

	int TimeOfDay::asSeconds() const noexcept
	{
		return (((hour * 60) + minute) * 60) + second;
	}

	std::string TimeOfDay::toString() const noexcept
	{
		std::string str = soup::string::lpad(fmt::to_string(hour), 2, '0');
		str.push_back(':');
		str.append(soup::string::lpad(fmt::to_string(minute), 2, '0'));
		str.push_back(':');
		str.append(soup::string::lpad(fmt::to_string(second), 2, '0'));
		return str;
	}

	TimeOfDay TimeOfDay::fromString(const std::string& str)
	{
		return fromString(StringUtils::utf8_to_utf16(str));
	}

	TimeOfDay TimeOfDay::fromString(const std::wstring& str)
	{
		TimeOfDay tod{};
		std::optional<int> hour;
		auto min_sep = str.find(L':');
		if (min_sep == std::string::npos)
		{
			hour = str2int<int>(str);
		}
		else
		{
			hour = str2int<int>(str.substr(0, min_sep));
		}
		if (hour.has_value())
		{
			tod.hour = hour.value();
			if (min_sep != std::string::npos && min_sep + 1 < str.size())
			{
				++min_sep;
				std::optional<int> minute;
				auto sec_sep = str.find(L':', min_sep);
				if (min_sep == std::string::npos)
				{
					minute = str2int<int>(str);
				}
				else
				{
					minute = str2int<int>(str.substr(min_sep, sec_sep - min_sep));
				}
				if (minute.has_value())
				{
					tod.minute = minute.value();
					if (sec_sep != std::string::npos && sec_sep + 1 < str.size())
					{
						auto second = str2int<int>(str.substr(sec_sep + 1));
						if (second.has_value())
						{
							tod.second = second.value();
						}
					}
				}
			}
		}
		else
		{
			tod.hour = -1;
		}
		return tod;
	}
}
