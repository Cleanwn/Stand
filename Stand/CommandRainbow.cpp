#include "CommandRainbow.hpp"

#include <fmt/format.h>

#include "get_current_time_millis.hpp"
#include "regular_event.hpp"

namespace Stand
{
	CommandRainbow::CommandRainbow(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, CommandColourCustom* colour, hue_change_handler_t hue_change_handler)
		: CommandSlider(parent, std::move(menu_name), std::move(command_names), LOC("LGBT_H"), 0, 1000, 0, 1, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, false, {}, COMMAND_SLIDER_RAINBOW), colour(colour), hue_change_handler(hue_change_handler)
	{
	}

	void CommandRainbow::onChange(Click& click, int prev_value)
	{
		ensureScriptThread(click, [this]
		{
			const auto rainbow_entry = active_rainbows.find(this);
			if (value > 0)
			{
				colour->rainbow_active = true;
				const auto llvalue = (long long)value;
				const auto tickrate_hue_map_entry = tickrate_hue_map.find(llvalue);
				if (tickrate_hue_map_entry == tickrate_hue_map.end())
				{
					tickrate_hue_map.emplace(llvalue, colour->h->value);
				}
				else
				{
					Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
					colour->h->setValue(click, tickrate_hue_map_entry->second);
				}
				if (rainbow_entry == active_rainbows.end())
				{
					active_rainbows.emplace(this, RainbowData{ llvalue, get_current_time_millis() });
					if (ref_count++ == 0)
					{
						reScriptTickEvent::registerHandler(&onTick);
					}
				}
				else
				{
					rainbow_entry->second.ms_wait = (long long)value;
				}
			}
			else
			{
				colour->rainbow_active = false;
				if (rainbow_entry != active_rainbows.end())
				{
					active_rainbows.erase(rainbow_entry);
				}
				if (--ref_count == 0)
				{
					reScriptTickEvent::unregisterHandler(&onTick);
				}
			}
		});
	}

	std::string CommandRainbow::getState() const
	{
		if (value == 0)
		{
			return {};
		}
		auto state = fmt::to_string(value);
		if (!colour->supportsStateOperations())
		{
			state.append(" with saturation ").append(fmt::to_string(colour->s->value))
				 .append(" and value ").append(fmt::to_string(colour->v->value));
		}
		return state;
	}

	std::string CommandRainbow::getDefaultState() const
	{
		return {};
	}

	void CommandRainbow::setState(Click& click, const std::string& state)
	{
		auto sat_sep = state.find(" with saturation ");
		if (sat_sep == std::string::npos)
		{
			return CommandSlider::setState(click, state);
		}
		sat_sep += 17;
		auto val_sep = state.find(" and value ", sat_sep);
		colour->s->setState(click, state.substr(sat_sep, val_sep - sat_sep));
		colour->v->setState(click, state.substr(val_sep + 11));
		return CommandSlider::setState(click, state.substr(0, sat_sep));
	}

	void CommandRainbow::onTick()
	{
		const time_t this_frame = get_current_time_millis();
		for (auto& entry : active_rainbows)
		{
			entry.second.ms_passed += this_frame - entry.second.last_frame;
			entry.second.last_frame = this_frame;
			while (entry.second.ms_passed >= entry.second.ms_wait)
			{
				entry.second.ms_passed -= entry.second.ms_wait;
				Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
				if (entry.first->colour->h->value >= entry.first->colour->h->max_value)
				{
					entry.first->colour->h->setValue(click, 0);
				}
				else
				{
					entry.first->colour->h->setValue(click, entry.first->colour->h->value + 1);
				}
				entry.first->hue_change_handler(entry.first);
				tickrate_hue_map[entry.second.ms_wait] = entry.first->colour->h->value;
			}
		}
	}
}
