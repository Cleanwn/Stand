#pragma once

#include "CommandListActionItem.hpp"
#include "CommandLockWeaponsLivery.hpp"
#include "CommandLockWeaponsLiveryTint.hpp"
#include "CommandSlider.hpp"
#include "CommandToggleLockWeapon.hpp"
#include "CommandToggleWeaponComponent.hpp"
#include "StringUtils.hpp"
#include "weapon_components.hpp"

namespace Stand
{
	class CommandListLockWeapon : public CommandList
	{
	private:
		CommandToggle* toggle = nullptr;
		CommandSlider* colour = nullptr;
		CommandListSelect* livery = nullptr;
		CommandListSelect* livery_tint = nullptr;

		std::string config_name;
	public:
		Hash weapon;

		explicit CommandListLockWeapon(CommandList* const parent, Label&& menu_name, std::string&& config_name, Hash weapon)
			: CommandList(parent, std::move(menu_name), {}), config_name(std::move(config_name)), weapon(weapon)
		{
			std::string command_name = this->config_name;
			StringUtils::simplify(command_name);
			toggle = createChild<CommandToggleLockWeapon>(LIT(this->menu_name.getLocalisedUtf8()), combineCommandNames(CMDNAMES_OBF("lockweapon"), { utf8ToCmdName(command_name) }), this->config_name, this->weapon);
			createChild<CommandDivider>(LOC("TINT"));
			colour = createChild<CommandSlider>(LOC("TINT"), combineCommandNames(toggle->command_names.at(0), CMDNAMES_OBF("tint")), NOLABEL, 0, 32, 0, 1);

			// Liveries
			for (auto& livery_group : g_weapon_liveries)
			{
				if (livery_group.first == weapon)
				{
					livery = createChild<CommandLockWeaponsLivery>(toggle->command_names.at(0), livery_group.second);
					livery_tint = createChild<CommandLockWeaponsLiveryTint>(toggle->command_names.at(0));

					break;
				}
			}

			// Components
			bool made_divider = false;
			for (const auto& component : g_weapon_components)
			{
				if (WEAPON::DOES_WEAPON_TAKE_WEAPON_COMPONENT(weapon, component.hash))
				{
					if (!made_divider)
					{
						createChild<CommandDivider>(LOC("WEAPCOMPS"));
						made_divider = true;
					}

					createChild<CommandToggleWeaponComponent>(component, toggle->command_names.at(0), weapon);
				}
			}
		}

		[[nodiscard]] std::string getNameForConfig() const final
		{
			return config_name;
		}

		[[nodiscard]] bool isEnabled()
		{
			return toggle->m_on;
		}

		[[nodiscard]] int getTint()
		{
			return colour->value;
		}

		[[nodiscard]] std::unordered_map<Hash, bool> getComponents()
		{
			std::unordered_map<Hash, bool> res;

			forEachPhysicalChild([&res, this](CommandPhysical* cmd)
			{
				if (cmd->isToggle() && cmd != toggle)
				{
					auto c = cmd->as<CommandToggleWeaponComponent>();
					res.emplace(c->component, c->m_on);
				}
			});

			return res;
		}

		[[nodiscard]] Hash getLivery()
		{
			if (livery)
			{
				return (Hash)livery->value;
			}
			
			return 0;
		}

		[[nodiscard]] uint8_t getLiveryTint()
		{
			if (livery_tint)
			{
				return (uint8_t)livery_tint->value;
			}

			return 0;
		}

		[[nodiscard]] std::vector<Hash> getAllLiveries()
		{
			std::vector<Hash> res;

			if (livery)
			{
				livery->forEachChildAsPhysical([&res](CommandPhysical* cmd)
				{
					res.emplace_back((Hash)cmd->as<CommandListActionItem>()->value);

					CONSUMER_CONTINUE;
				});
			}

			return res;
		}
	};
}