#pragma once

#include "CommandListSelect.hpp"

#include "StringUtils.hpp"
#include "weapon_components.hpp"

namespace Stand
{
	class CommandLockWeaponsLivery : public CommandListSelect
	{
	private:
		[[nodiscard]] static std::vector<CommandListActionItemData> getOptions(std::vector<WeaponComponent>& liveries)
		{
			std::vector<CommandListActionItemData> res { CommandListActionItemData(0, LOC("NONE"), CMDNAMES("none")) };

			for (const auto& livery : liveries)
			{
				res.emplace_back(CommandListActionItemData(livery.hash.operator hash_t(), std::move(livery.getName()), { utf8ToCmdName(StringUtils::simplify(livery.getAmericanName())) }));
			}

			return res;
		}

	public:
		explicit CommandLockWeaponsLivery(CommandList* const parent, const CommandName& command_names_prefix, std::vector<WeaponComponent>& liveries)
			: CommandListSelect(parent, LOC("VEHMOD48"), combineCommandNames(command_names_prefix, CMDNAMES("livery")), NOLABEL, getOptions(liveries), 0)
		{
		}
	};
}