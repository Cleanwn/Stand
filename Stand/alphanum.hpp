#pragma once

#include <string>

#include "is_number_char.hpp"

namespace Stand
{
	// http://www.davekoelle.com/alphanum.html

	template <typename Char>
	inline int alphanum_impl(const Char* l, const Char* r)
	{
		enum mode_t { STRING, NUMBER } mode = STRING;

		while (*l && *r)
		{
			if (mode == STRING)
			{
				Char l_char, r_char;
				while ((l_char = *l) && (r_char = *r))
				{
					// check if this are digit characters
					const bool l_digit = is_number_char(l_char), r_digit = is_number_char(r_char);
					// if both characters are digits, we continue in NUMBER mode
					if (l_digit && r_digit)
					{
						mode = NUMBER;
						break;
					}
					// if only the left character is a digit, we have a result
					if (l_digit) return -1;
					// if only the right character is a digit, we have a result
					if (r_digit) return +1;
					// compute the difference of both characters
					const int diff = l_char - r_char;
					// if they differ we have a result
					if (diff != 0) return diff;
					// otherwise process the next characters
					++l;
					++r;
				}
			}
			else // mode==NUMBER
			{
				// get the left number
				unsigned long long l_int = 0;
				while (*l && is_number_char(*l))
				{
					// TODO: this can overflow
					l_int = l_int * 10 + *l - '0';
					++l;
				}

				// get the right number
				unsigned long long r_int = 0;
				while (*r && is_number_char(*r))
				{
					// TODO: this can overflow
					r_int = r_int * 10 + *r - '0';
					++r;
				}

				// if the difference is not equal to zero, we have a comparison result
				const long long diff = l_int - r_int;
				if (diff != 0)
					return diff < 0 ? -1 : +1;

				// otherwise we process the next substring in STRING mode
				mode = STRING;
			}
		}

		if (*r) return -1;
		if (*l) return +1;
		return 0;
	}

	/*inline int alphanum_comp(const std::string& l, const std::string& r)
	{
		return alphanum_impl(l.c_str(), r.c_str());
	}

	inline int alphanum_comp(const std::wstring& l, const std::wstring& r)
	{
		return alphanum_impl(l.c_str(), r.c_str());
	}*/

	inline bool alphanum_less(const std::string& l, const std::string& r)
	{
		return alphanum_impl(l.c_str(), r.c_str()) < 0;
	}

	inline bool alphanum_less(const std::wstring& l, const std::wstring& r)
	{
		return alphanum_impl(l.c_str(), r.c_str()) < 0;
	}
}
