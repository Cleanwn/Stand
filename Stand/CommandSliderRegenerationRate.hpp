#pragma once

#include "CommandSliderFloat.hpp"

namespace Stand
{
	class CommandSliderRegenerationRate : public CommandSliderFloat
	{		
	public:
		enum RegenType : uint8_t
		{
			HEALTH,
			ARMOUR
		};

	private:
		RegenType regen_type;
		time_t last_interval = 0;

	public:
		CommandToggle* only_in_cover = nullptr;

		explicit CommandSliderRegenerationRate(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, RegenType regen_type)
			: CommandSliderFloat(parent, std::move(menu_name), std::move(command_names), std::move(help_text), 0, 5000, 0, 50), regen_type(regen_type)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				if (num == 0)
				{
					return LANG_GET_W("OVRDOFF");
				}
			}

			return CommandSliderFloat::formatNumber(num, allow_replacements);
		}

		void onChange(Click& click, int prev_value) final
		{
			const auto fvalue = getFloatValue();
			if (fvalue != 0.0f)
			{
				registerScriptTickEventHandler([this, fvalue]
				{
					HANDLER_CHECK(fvalue == getFloatValue());
					if (!g_player_ped.isDead()
						&& GET_MILLIS_SINCE(last_interval) >= 1000
						&& (!only_in_cover->m_on || PED::IS_PED_IN_COVER(g_player_ped, FALSE))
						)
					{
						if (regen_type == RegenType::HEALTH)
						{
							auto old = g_player_ped.getHealth();
							auto max = g_player_ped.getMaxHealth();
							auto now = old + fvalue;
							if (auto new_value = now > max ? max : now; new_value != old)
							{
								g_player_ped.setHealth(new_value);
							}
						}
						else
						{
							auto old = g_player_ped.getArmour();
							auto max = g_player_ped.getMaxArmour();
							auto now = old + fvalue;
							if (auto new_value = now > max ? max : now; new_value != old)
							{
								g_player_ped.setArmour(new_value);
							}
						}

						last_interval = get_current_time_millis();
					}
					HANDLER_END;
				});
			}
		}
	};
}