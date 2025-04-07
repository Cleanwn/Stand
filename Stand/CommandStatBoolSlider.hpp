#pragma once

#include "CommandStatSlider.hpp"

namespace Stand
{
	class CommandStatBoolSlider : public CommandStatSlider<CommandSlider>
	{
	public:
		explicit CommandStatBoolSlider(CommandList* const parent, Stat& stat)
			: CommandStatSlider(parent, stat, 0, 1)
		{
		}

		std::wstring formatNumber(int num, bool allow_replacements = true) const final
		{
			if (allow_replacements)
			{
				return num == 0 ? LANG_GET_W("NO") : LANG_GET_W("YES");
			}

			return CommandSlider::formatNumber(num, allow_replacements);
		}

	protected:
		void setStat(Hash stat) final
		{
			STATS::STAT_SET_BOOL(stat, this->value, TRUE);
		}

		void getStat(Hash stat, int* value) final
		{
			STATS::STAT_GET_BOOL(stat, value, -1);
		}
	};
}