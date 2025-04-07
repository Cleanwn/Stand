#pragma once

#include "CommandListSelect.hpp"

#include "drawDebugText.hpp"

namespace Stand
{
	class CommandCountryBreakdown : public CommandListSelect
	{
	public:
		explicit CommandCountryBreakdown(CommandList* const parent)
			: CommandListSelect(parent, LOC("CTRYBRKDWN"), { CMDNAME("infocountrybreakdown") }, LOC("CTRYBRKDWN_H"), {
				{0, LOC("DOFF")},
				{1, LOC("SRTBYCNT")},
				{2, LOC("SRTBYCTRYNME")},
			}, 0)
		{
		}

		void onChange(Click& click, long long prev_value) final
		{
			if (prev_value == 0)
			{
				registerScriptTickEventHandler(click, [this]
				{
					HANDLER_CHECK(value != 0)
					std::vector<std::pair<std::string, compactplayer_t>> breakdown{};
					for (auto p : AbstractPlayer::listAll())
					{
						auto ctry = p.getCountry();
						if (!ctry.empty())
						{
							auto e = std::find_if(breakdown.begin(), breakdown.end(), [&](const std::pair<std::string, compactplayer_t>& e)
							{
								return e.first == ctry;
							});
							if (e == breakdown.end())
							{
								breakdown.emplace_back(ctry, 1);
							}
							else
							{
								++e->second;
							}
						}
					}
					if (value == 1)
					{
						std::sort(breakdown.begin(), breakdown.end(), [](const std::pair<std::string, compactplayer_t>& a, const std::pair<std::string, compactplayer_t>& b)
						{
							auto cmp = (a.second <=> b.second);
							if (cmp > 0)
							{
								return true;
							}
							if (cmp < 0)
							{
								return false;
							}
							// Sort by country name if count is equal
							return a.first < b.first;
						});
					}
					else
					{
						std::sort(breakdown.begin(), breakdown.end(), [](const std::pair<std::string, compactplayer_t>& a, const std::pair<std::string, compactplayer_t>& b)
						{
							return a.first < b.first;
						});
					}
					for (const auto& e : breakdown)
					{
						drawInfoNumber(e.second, e.first);
					}
					HANDLER_END;
				});
			}
		}
	};
}
