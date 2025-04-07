#include "extract_delimited_string_with_escaping.hpp"

namespace Stand
{
	std::string extract_delimited_string_with_escaping(std::string& str, const char delimiter)
	{
		std::string extract;
		bool escape = false;
		while (!str.empty())
		{
			auto c = str.at(0);
			str.erase(0, 1);
			if (escape)
			{
				extract.push_back(c);
				escape = false;
			}
			else if (c == '\\')
			{
				escape = true;
			}
			else if (c == delimiter)
			{
				break;
			}
			else
			{
				extract.push_back(c);
			}
		}
		return extract;
	}

	std::wstring extract_delimited_string_with_escaping(std::wstring& str, const wchar_t delimiter)
	{
		std::wstring extract;
		bool escape = false;
		while (!str.empty())
		{
			auto c = str.at(0);
			str.erase(0, 1);
			if (escape)
			{
				extract.push_back(c);
				escape = false;
			}
			else if (c == L'\\')
			{
				escape = true;
			}
			else if (c == delimiter)
			{
				break;
			}
			else
			{
				extract.push_back(c);
			}
		}
		if (escape)
		{
			extract.push_back(L'\\');
		}
		return extract;
	}
}
