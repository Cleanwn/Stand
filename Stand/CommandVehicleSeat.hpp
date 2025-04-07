#pragma once

namespace Stand
{
	class CommandVehicleSeat : public CommandSlider
	{
	public:
		explicit CommandVehicleSeat(CommandList* const parent)
			: CommandSlider(parent, LOC("SETSEAT"), CMDNAMES_OBF("setseat"), NOLABEL, -1, 7, -1, 1, CMDFLAGS_SLIDER, COMMANDPERM_USERONLY, true)
		{
		}

		[[nodiscard]] std::wstring formatNumber(int num, bool allow_replacements = true) const final
		{
			if (allow_replacements)
			{
				if (num == -1)
				{
					std::wstring str = L"-1 (";
					str.append(LANG_GET_W("DRV"));
					str.push_back(L')');
					return str;
				}
			}
			return CommandSlider::formatNumber(num);
		}

		void onClick(Click& click) final
		{
			if (auto veh = Util::getVehicle(click); veh.isValid())
			{
				PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, value);
			}
		}
	};
}
