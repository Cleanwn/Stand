#include "CommandListWeaponComponents.hpp"

#include "AbstractEntity.hpp"
#include "gta_ped.hpp"
#include "natives.hpp"
#include "weapon_components.hpp"

#include "CommandWeaponComponent.hpp"

namespace Stand
{
	CommandListWeaponComponents::CommandListWeaponComponents(CommandList* const parent)
		: CommandListViewport(parent, LOC("WEAPCOMPS"), {}, LOC("WEAPCOMPS_H"))
	{
		for (const auto& wc : g_weapon_components)
		{
			this->createChild<CommandWeaponComponent>(wc.getName(), wc.hash.operator hash_t());
		}
		for (const auto& wl : g_weapon_liveries)
		{
			for (const auto& wc : wl.second)
			{
				this->createChild<CommandWeaponComponent>(wc.getName(), wc.hash.operator hash_t());
			}
		}
	}

	void CommandListWeaponComponents::onTickAsActiveList()
	{
		if (auto ped = g_player_ped.getCPed(); ped && ped->weapon_manager)
		{
			const hash_t weapon = g_player_ped.getCPed()->weapon_manager->selector.selected_weapon_hash;
			if (weapon != this->weapon)
			{
				this->weapon = weapon;

				for (const auto& child : children)
				{
					auto cmd = child->as<CommandWeaponComponent>();
					setVisibleBulk(cmd, WEAPON::DOES_WEAPON_TAKE_WEAPON_COMPONENT(weapon, cmd->component));
				}
				processChildrenVisibilityUpdate();
			}
		}

		CommandListViewport::onTickAsActiveList();
	}
}
