#include "format_time_since_1970_for_user_locale.hpp"

#include <memory>

#include <windows.h>

namespace Stand
{
	std::wstring format_time_since_1970_for_user_locale(time_t time, bool only_time)
	{
		// Adjust for user timzone
		{
			TIME_ZONE_INFORMATION tzi = { 0 };
			if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT)
			{
				time -= (tzi.DaylightBias * 60);
			}
			time -= (tzi.Bias * 60);
		}

		// Convert to system time
		SYSTEMTIME st;
		time = Int32x32To64(time, 10000000) + 116444736000000000;
		if (!FileTimeToSystemTime((FILETIME*)&time, &st))
		{
			return L"Invalid";
		}

		// Format
		std::unique_ptr<wchar_t[]> datebuf;

		if (!only_time)
		{
			auto datelen = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, nullptr, 0, nullptr);
			datebuf = std::make_unique<wchar_t[]>(datelen);
			GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, datebuf.get(), datelen, nullptr);
		}

		auto timelen = GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, nullptr, nullptr, 0);
		auto timebuf = std::make_unique<wchar_t[]>(timelen);
		GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, nullptr, timebuf.get(), timelen);

		if (only_time)
		{
			return std::wstring(timebuf.get());
		}
		else
		{
			return std::wstring(datebuf.get()).append(L" ").append(timebuf.get());
		}
	}
}
