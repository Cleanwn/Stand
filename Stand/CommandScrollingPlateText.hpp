#pragma once

#include "CommandToggle.hpp"

#include "get_current_time_millis.hpp"

namespace Stand
{
	class CommandScrollingPlateText : public CommandToggle
	{
		CommandInput* plate_string {};
		CommandSlider* plate_scroll_delay {};
		size_t plate_text_cursor {};
		time_t timer {};

		std::string wrapped_string_window(std::string const& str, size_t const window_start, size_t const window_width) {
			std::string retStr {};

			if(!str.empty())
			{
				for(auto cursor = window_start; cursor != window_start + window_width; ++cursor)
				{
					retStr += str[cursor % str.size()];
				}
			}

			return retStr;
		}

		[[nodiscard]] std::string prep_plate(std::string str) const
		{
			std::for_each(str.begin(), str.end(), [](char& c)
			{
				if(c == ' ')
				{
					c = '.';
				}
			});

			return str;
		}
	
		void wrapped_increment(size_t& value, size_t const max, size_t const wrap_to = 0) const
		{
			value = ++value >= max
				? wrap_to
#include "get_current_time_millis.hpp"
				: value;
		}

		bool delay_finished()
		{
			auto const now = get_current_time_millis();

			if (now > timer + static_cast<time_t>(plate_scroll_delay->value))
			{
				timer = now;
				return true;
			}

			return false;
		}
	
	public:
		explicit CommandScrollingPlateText(CommandList* const parent, CommandInput* const plate_string, CommandSlider* const plate_scroll_delay)
			: CommandToggle(parent, LOC("SCRLTXT"), CMDNAMES("platescroll"), NOLABEL), plate_string(plate_string), plate_scroll_delay(plate_scroll_delay) {}

		void onChange(Click& click) final {
			if(m_on)
			{
				registerScriptTickEventHandler(click, [this]
				{
					if(m_on)
					{
						if (g_player_veh.isValid() && delay_finished())
						{						
							wrapped_increment(plate_text_cursor, plate_string->value.size());
							auto const plate = prep_plate(std::move(wrapped_string_window(plate_string->value, plate_text_cursor, 8)));

							g_player_veh.setPlateText(plate);
						}
						return true;
					}
					return false;
				});
			}
		}
	};
}
