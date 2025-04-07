#pragma once

#include "CommandListWeapons.hpp"
#include "CommandOnPlayer.hpp"

#include "AbstractEntity.hpp"
#include "CommandLambdaActionScript.hpp"

namespace Stand
{
	class CommandPlayerArm : public CommandOnPlayer<CommandListWeapons>
	{
	public:
		explicit CommandPlayerArm(CommandList* const parent)
			: CommandOnPlayer(parent, LOC("PLYARM_P"), CMDNAMES_OBF("arm"))
		{
			PC_PTR->registerCommandNames(this);
			populate();
		}

		void createCommand(CommandList* list, Label&& menu_name, std::string&& config_name, std::vector<hash_t>&& weapons) final
		{
			if (weapons.size() > 1)
			{
				list->createChild<CommandLambdaActionScript>(LOC("PLYARM"), getSuffixedCommandNames(CMDNAMES_OBF("all")), NOLABEL, [this](const Click& click)
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						p.getPed().giveAllWeapons();
					}
				}, 0, COMMANDPERM_FRIENDLY);
			}
			else
			{
				CommandName command_name = utf8ToCmdName(config_name);
				StringUtils::simplify(command_name);
				auto command_names = getSuffixedCommandNames(std::move(command_name));

				list->createChild<CommandLambdaActionScript>(std::move(menu_name), std::move(command_names), NOLABEL, [this, weapons{ std::move(weapons) }](const Click& click)
				{
					for (const auto& p : PP_PTR->getPlayers())
					{
						p.getPed().giveWeapons(std::vector<hash_t>((weapons)));
					}
				}, 0, COMMANDPERM_FRIENDLY);
			}
		}
	};
}
