#pragma once

#include "CommandSliderFloat.hpp"

#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandWeaponRange : public CommandSliderFloat
	{
	public:
		explicit CommandWeaponRange(CommandList* const parent)
			: CommandSliderFloat(parent, LOC("RNGMUL"), { CMDNAME("rangemultiplier"), CMDNAME("weaponrangemultiplier") }, NOLABEL, 0, 150, 100, 10)
		{
		}

		void onChange(Click& click, int prev_value) final
		{
			ensureScriptThread(click, [this]
			{
				WeaponMgr::weapon_range_multiplier = getFloatValue();
			});
		}
	};
}
