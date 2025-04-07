#include "TimedTextCollection.hpp"

#include "str2int.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	TimedTextCollection TimedTextCollection::fromLRC(std::ifstream& f) noexcept
	{
		TimedTextCollection c{};
		std::string line;
		while (std::getline(f, line))
		{
			if (line.empty())
			{
				continue;
			}
			if (line.at(0) != '[')
			{
				continue;
			}
			auto sep_i = line.find(']', 1);
			if (sep_i == std::string::npos)
			{
				continue;
			}
			auto tag = line.substr(1, sep_i - 1);
			if (!tag.empty())
			{
				auto it = &*tag.cbegin();
				auto m = str2int_it<int>(it);
				if (it < &*tag.cend() && *--it == ':')
				{
					auto s = str2int_it<uint8_t>(++it);
					if (it < &*tag.cend() && *--it == '.')
					{
						auto x = str2int_it<uint8_t>(++it);
						c.entries.emplace_back(TimedText{
							((((m * 60) + s) * 100) + x) * 10,
							line.substr(sep_i + 1)
						});
					}
				}
			}
		}
		return c;
	}

	void TimedTextCollection::eraseAdvertisements() noexcept
	{
		constexpr const char* sites[] = {
			"rentanadvisercom",
		};
		for (auto i = entries.cbegin(); i != entries.cend(); )
		{
			auto simple_text = StringUtils::simplify(i->text);
			if (simple_text.substr(0, 2) == "by")
			{
				simple_text.erase(0, 2);
			}
			if (simple_text.substr(0, 3) == "www")
			{
				simple_text.erase(0, 3);
			}
			bool is_ad = false;
			for (const auto& site : sites)
			{
				if (simple_text == site)
				{
					is_ad = true;
					break;
				}
			}
			if (is_ad)
			{
				i = entries.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void TimedTextCollection::setStartOffset(time_t offset_millis) noexcept
	{
		if (entries.empty())
		{
			return;
		}
		offset_millis -= entries.at(0).offset_millis;
		for (auto& e : entries)
		{
			e.offset_millis += offset_millis;
		}
	}

	TimedText TimedTextCollection::getTextAtOffset(time_t offset_millis) noexcept
	{
		TimedText text{};
		for (const auto& e : entries)
		{
			if (e.offset_millis > offset_millis)
			{
				break;
			}
			text = e;
		}
		return text;
	}
}
