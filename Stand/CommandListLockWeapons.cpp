#pragma once

#include "CommandToggleLockWeapons.hpp"

#include "AbstractEntity.hpp"
#include "CommandDivider.hpp"
#include "CommandListCommandBoxPresets.hpp"
#include "CommandListLockWeapon.hpp"
#include "CommandLockWeaponsSavePreset.hpp"
#include "CommandOnlyRemoveSelectedWeapons.hpp"
#include "CommandTogglePointer.hpp"
#include "CommandToggleLockWeapon.hpp"
#include "get_current_time_millis.hpp"
#include "is_session.hpp"
#include "weapons.hpp"

namespace Stand
{
	CommandListLockWeapons::CommandListLockWeapons(CommandList* const parent)
		: CommandListWeapons(parent, LOC("WEAPLOCK"))
	{
		this->createChild<CommandToggleLockWeapons>();
		this->createChild<CommandListCommandBoxPresets<CommandLockWeaponsSavePreset>>(CMDNAMES_OBF("lockweaponpresets"), "Weapon Loadouts", LOC("LOADS"), LOC("SVLOAD"), LOC("VWLOADS"), LOC("SVLOAD_H_WEAP"));
		invert = this->createChild<CommandOnlyRemoveSelectedWeapons>();
		this->createChild<CommandDivider>(LOC("WPN"));
		populate();
	}

	void CommandListLockWeapons::onTickEnabled()
	{
		// Preserve our default online loadout throughout transitions.
		// Slightly problematic when moving from MP->SP, seems we keep some random weapons?
		// The handler is not running when this problem occurs, so I assume it's a game issue.
		{
			if (is_session_transition_active(true))
			{
				time_since_last_transition = get_current_time_millis();
				return;
			}

			if (GET_MILLIS_SINCE(time_since_last_transition) <= 2300)
			{
				children.at(0)->as<CommandToggleLockWeapons>()->saveLoadout();
				return;
			}
		}

		std::vector<hash_t> to_remove{};
		const auto our_weapons = getAllOfOurWeapons();

		for (const auto& weapon : our_weapons)
		{
			if (invert->m_on ? shouldHaveWeapon(weapon) : !shouldHaveWeapon(weapon))
			{
				to_remove.emplace_back(weapon);
			}
		}

		if (!invert->m_on)
		{
			giveWeapons();
		}

		g_player_ped.removeWeapons(std::move(to_remove));
	}

	void CommandListLockWeapons::changeWeapon(hash_t weapon, bool state)
	{
		if (state)
		{
			weapons.emplace_back(weapon);
		}
		else
		{
			weapons.erase(std::find(weapons.begin(), weapons.end(), weapon));
		}
	}

	void CommandListLockWeapons::giveWeapons() const
	{
		g_player_ped.giveWeapons(std::vector<hash_t>(weapons));

#if false
		// Allowing saved components with locked components would be confusing and make 'removing components' difficult.
		// Need to pick a side here. This forces people to reconfigure their locked loadouts, but it's only temporary loss.
		const auto saved_loadout = children.at(0)->as<CommandToggleLockWeapons>()->saved_loadout;

		// Apply saved components, if any.
		for (const hash_t weapon_hash : weapons)
		{
			saved_loadout.applySingleWeapon(g_player_ped, weapon_hash);
		}
#endif

		// Lock Tint, Components, and Livery
		forEachPhysicalChild([this](CommandPhysical* weapon_category) // In the list with "Rifles", "Pistols", etc.
		{
			if (weapon_category->isList() && weapon_category->getMenuName() != ATSTRINGHASH("LOADS"))
			{
				weapon_category->as<CommandList>()->forEachPhysicalChild([this](CommandPhysical* weapon_name) // In the list with "AP Pistol", "Carbine Rifle", etc.
				{
					auto cmd = weapon_name->as<CommandListLockWeapon>();

					if (cmd->isEnabled() && shouldHaveWeapon(cmd->weapon))
					{
						WEAPON::SET_PED_WEAPON_TINT_INDEX(g_player_ped, cmd->weapon, cmd->getTint());

						for (const auto& component : cmd->getComponents())
						{
							auto component_hash = component.first;
							bool should_equip = component.second;

							if (should_equip)
							{
								WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(g_player_ped, cmd->weapon, component_hash);
							}
							else
							{
								WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(g_player_ped, cmd->weapon, component_hash);
							}
						}

						if (auto livery = cmd->getLivery(); livery != 0)
						{
							WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(g_player_ped, cmd->weapon, livery);
							WEAPON::SET_PED_WEAPON_COMPONENT_TINT_INDEX(g_player_ped, cmd->weapon, livery, cmd->getLiveryTint());
						}
						else
						{
							for (auto& livery : cmd->getAllLiveries())
							{
								WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(g_player_ped, cmd->weapon, livery);
							}
						}
					}
				});
			}
		});
	}

	void CommandListLockWeapons::createCommand(CommandList* list, Label&& menu_name, std::string&& config_name, std::vector<hash_t>&& weapons)
	{
		if (weapons.size() == 1)
		{
			list->createChild<CommandListLockWeapon>(std::move(menu_name), std::move(config_name), weapons.at(0));
		}
	}

	bool CommandListLockWeapons::shouldHaveWeapon(hash_t weapon) const
	{
		return std::find(weapons.begin(), weapons.end(), weapon) != weapons.end();
	}

	std::vector<hash_t> CommandListLockWeapons::getAllOfOurWeapons() const
	{
		std::vector<hash_t> weapons{};
		const auto hashes = Weapon::getAllHashes();

		for (const auto& hash : hashes)
		{
			if (WEAPON::HAS_PED_GOT_WEAPON(g_player_ped, hash, FALSE))
			{
				weapons.emplace_back(hash);
			}
		}

		return weapons;
	}
}