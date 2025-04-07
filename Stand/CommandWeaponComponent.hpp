#pragma once

#include "CommandToggleNoCorrelation.hpp"

namespace Stand
{
	class CommandWeaponComponent : public CommandToggleNoCorrelation
	{
	public:
		const hash_t component;

		explicit CommandWeaponComponent(CommandList* const parent, Label&& menu_name, hash_t component)
			: CommandToggleNoCorrelation(parent, std::move(menu_name), {}, NOLABEL, false, (CMDFLAGS_TOGGLE & ~CMDFLAG_SUPPORTS_STATE_OPERATIONS) | CMDFLAG_TEMPORARY | CMDFLAG_STATE_AND_CHECK_FINISHLIST | CMDFLAG_FEATURELIST_SKIP), component(component)
		{
		}

		[[nodiscard]] hash_t getWeapon() const noexcept
		{
			return parent->as<CommandListWeaponComponents>()->weapon;
		}

		void onEnable(Click& click) final
		{
			WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(g_player_ped, getWeapon(), component);
		}

		void onDisable(Click& click) final
		{
			WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(g_player_ped, getWeapon(), component);
		}

		void onTickInGameViewport() final
		{
			if (!isConcealed())
			{
				setOnIndication(WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(g_player_ped, getWeapon(), component));
			}
		}
	};
}
