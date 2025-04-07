#pragma once

#include "CommandToggle.hpp"

#include "WeaponMgr.hpp"

namespace Stand
{
	class CommandNoRecoil : public CommandToggle
	{
	public:
		explicit CommandNoRecoil(CommandList* const parent)
			: CommandToggle(parent, LOC("NORECL"), CMDNAMES("norecoil"))
		{
		}

		void onChange(Click& click) final
		{
			WeaponMgr::weapon_recoil_multiplier = this->m_on ? 0.0f : 1.0f;
			WeaponMgr::weapon_recoil_shake_multiplier = this->m_on ? 9999 : 1;
			WeaponMgr::weapon_recoil_shake_hash_identity = this->m_on ? 0 : 1;
		}
	};
}