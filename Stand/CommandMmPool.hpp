#pragma once

#include "CommandSlider.hpp"

#include "Hooking.hpp"

namespace Stand
{
	class CommandMmPool : public CommandSlider
	{
	public:
		explicit CommandMmPool(CommandList* const parent)
			: CommandSlider(parent, LOC("MMPOOL"), { CMDNAME("mmpoolslider") }, NOLABEL, INT_MIN, INT_MAX, 0)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements) const final
		{
			if (allow_replacements)
			{
				if (num == 0)
				{
					return LANG_GET_W("MMPOOL_NRM");
				}
				if (num == 1)
				{
					return LANG_GET_W("MMPOOL_CHEAT");
				}
				if (num == 2)
				{
					return LANG_GET_W("MMPOOL_BADBOY");
				}
			}
			return CommandSlider::formatNumber(num);
		}

		void onChange(Click& click, int prev_value) final
		{
			g_hooking.spoofed_matchmaking_pool = value;
		}
	};
}
