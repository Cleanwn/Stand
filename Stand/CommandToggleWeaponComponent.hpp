#pragma once

#include "CommandToggle.hpp"

#include "StringUtils.hpp"
#include "weapon_components.hpp"

namespace Stand
{
	class CommandToggleWeaponComponent : public CommandToggle
	{
	public:
		Hash weapon;
		Hash component;
		std::string config_name;

		explicit CommandToggleWeaponComponent(CommandList* const parent, const WeaponComponent& component, const CommandName& command_name_prefix, Hash weapon)
			: CommandToggle(parent, component.getName(), {}), weapon(weapon), component(component.hash), config_name(component.getAmericanName())
		{
			this->command_names = combineCommandNames(command_name_prefix, { utf8ToCmdName(StringUtils::simplify(component.getAmericanName())) });
		}

		[[nodiscard]] std::string getNameForConfig() const final
		{
			return config_name;
		}
	};
}