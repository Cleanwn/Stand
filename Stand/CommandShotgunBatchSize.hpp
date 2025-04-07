#pragma once

#include "CommandSlider.hpp"

#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandShotgunBatchSize : public CommandSlider
	{
	public:
		explicit CommandShotgunBatchSize(CommandList* const parent)
			: CommandSlider(parent, LOC("SHTGNSZE"), CMDNAMES_OBF("shotgunbatchsizemultiplier"), NOLABEL, 0, 16, 0, 1) // 16 seems like the max value. After that, it only lessens spread, for some reason.
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

			return CommandSlider::formatNumber(num);
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [this]
			{
				WeaponMgr::weapon_bullets_in_batch = (unsigned)this->value;
			});
		}
	};
}
