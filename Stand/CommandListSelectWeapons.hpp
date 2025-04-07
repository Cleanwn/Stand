#pragma once

#include "CommandListSelect.hpp"

#include "StringUtils.hpp"
#include "weapons.hpp"

namespace Stand
{
	class CommandListSelectWeapons : public CommandListSelect
	{
	private:
		static void addOption(std::vector<CommandListActionItemData>& weap_options, const Weapon& weapon)
		{
			weap_options.emplace_back(CommandListActionItemData(
				weapon.hash,
				LIT(weapon.getName()),
				{ utf8ToCmdName(StringUtils::simplify(weapon.american_name)) },
				NOLABEL,
				COMMANDPERM_USERONLY,
				CMDFLAG_FEATURELIST_FINISHLIST,
				LIT(weapon.getCategoryName())
			));
		}

		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions()
		{
			std::vector<CommandListActionItemData> weap_options{};
			addOption(weap_options, Weapon::weapon_unarmed);
			for (const auto& weapon : Weapon::weapons)
			{
				// CommandListSelectWeapons is currently only used for non-player peds, so throwables make no sense, since they don't/can't use them.
				if (weapon.category != Weapon::THROWABLE)
				{
					addOption(weap_options, weapon);
				}
			}
			return weap_options;
		}

	public:
		explicit CommandListSelectWeapons(CommandList* const parent, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, long long default_value = 0)
			: CommandListSelect(parent, LOC("WPN_SINGULAR"), std::move(command_names), std::move(help_text), getOptions(), default_value)
		{
		}

		explicit CommandListSelectWeapons(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, long long default_value = 0)
			: CommandListSelect(parent, std::move(menu_name), std::move(command_names), std::move(help_text), getOptions(), default_value)
		{
		}
	};
}
