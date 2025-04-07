#pragma once

#include "CommandToggle.hpp"

#include "CommandListLockWeapons.hpp"

namespace Stand
{
	class CommandToggleLockWeapon : public CommandToggle
	{
	private:
		std::string config_name;
		hash_t weapon;

	public:
		explicit CommandToggleLockWeapon(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, const std::string& config_name, hash_t weapon)
			: CommandToggle(parent, std::move(menu_name), std::move(command_names)), config_name(config_name), weapon(weapon)
		{
		}

		[[nodiscard]] std::string getNameForConfig() const final
		{
			return config_name;
		}

		void onChange(Click& click) final
		{
			CommandListWeapons::getHost(parent)->as<CommandListLockWeapons>()->changeWeapon(weapon, m_on);
		}
	};
}