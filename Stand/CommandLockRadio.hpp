#pragma once

#include "CommandToggle.hpp"

#include <fmt/format.h>

#include "get_current_time_millis.hpp"
#include "gta_input.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandLockRadio : public CommandToggleNoCorrelation
	{
	private:
		int station = -1;
		time_t last_q_press;

	public:
		explicit CommandLockRadio(CommandList* const parent)
			: CommandToggleNoCorrelation(parent, LOC("LKRD"), { CMDNAME("lockradio"), CMDNAME("radiolock") }, LOC("LKRD_H"))
		{
		}

		void onEnable(Click& click) final
		{
			ensureScriptThread(click, [=]
			{
				if (click.type != CLICK_BULK)
				{
					station = AUDIO::GET_PLAYER_RADIO_STATION_INDEX();
				}
				registerScriptTickEventHandler(TC_SCRIPT_NOYIELD, [=]()
				{
					if (!m_on)
					{
						return false;
					}
					if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_VEH_RADIO_WHEEL)
						|| PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_RADIO_WHEEL_UD)
						|| PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_RADIO_WHEEL_LR)
						)
					{
						last_q_press = get_current_time_millis();
					}
					if (last_q_press > get_current_time_millis() - 100)
					{
						station = AUDIO::GET_PLAYER_RADIO_STATION_INDEX();
					}
					else if (station == 255)
					{
						AUDIO::SET_RADIO_TO_STATION_NAME("OFF");
					}
					else
					{
						AUDIO::SET_RADIO_TO_STATION_INDEX(station);
					}
					return true;
				});
			});
		}

		std::string getState() const final
		{
			if (m_on)
			{
				return fmt::to_string(station);
			}
			return {};
		}

		std::string getDefaultState() const final
		{
			return {};
		}

		void setState(Click& click, const std::string& state) final
		{
			station = std::stoi(state);
			if (!m_on)
			{
				onClick(click);
			}
		}

		void applyDefaultState() final
		{
			m_on = false;
		}
	};
}
