#pragma once

namespace Stand
{
	template <typename T>
	struct IsolatedText
	{
		T text;
		std::vector<T> names;

		IsolatedText(const T& str)
		{
			text.reserve(str.size());
			for (auto i = str.begin(); i != str.end(); )
			{
				text.push_back(*i);
				if (*i == '{')
				{
					names.emplace_back();
					while (++i != str.end())
					{
						if (*i == '}')
						{
							break;
						}
						names.back().push_back(*i);
					}
				}
				else
				{
					++i;
				}
			}
		}

		[[nodiscard]] T deisolate() const
		{
			T str;
			str.reserve(text.size());
			auto names_i = names.begin();
			for (auto i = text.begin(); i != text.end(); ++i)
			{
				str.push_back(*i);
				if (*i == '{')
				{
					str.append(*names_i++);
				}
			}
			return str;
		}
	};
}
