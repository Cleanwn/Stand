#pragma once

#include "CommandToggle.hpp"

#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandNoSpread : public CommandToggle
	{
	public:
		explicit CommandNoSpread(CommandList* const parent)
			: CommandToggle(parent, LOC("NOSPRD"), CMDNAMES("nospread"))
		{
		}

		void onChange(Click& click) final
		{
			WeaponMgr::weapon_spread_multiplier = this->m_on ? 0.0f : 1.0f;
		}
	};
}