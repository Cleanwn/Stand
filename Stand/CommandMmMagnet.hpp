#pragma once

#include "CommandSlider.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandMmMagnet : public CommandSlider
	{
	public:
		explicit CommandMmMagnet(CommandList* const parent)
			: CommandSlider(parent, LOC("MMMGNT"), CMDNAMES_OBF("mmplayercount"), LOC("MMMGNT_H"), 0, 30, 0, 1)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (num == 0 && allow_replacements)
			{
				return LANG_GET_W("NONE");
			}

			return CommandSlider::formatNumber(num, allow_replacements);
		}

		void onChange(Click& click, int prev_value) final
		{
			g_hooking.magnet_matchmaking = value;
		}
	};
}
