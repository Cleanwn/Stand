#pragma once

#include "CommandListWeapons.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandRemoveGuns : public CommandListWeapons
	{
	public:
		explicit CommandRemoveGuns(CommandList* const parent)
			: CommandListWeapons(parent, LOC("DELGNS"), combineCommandNames(CMDNAME("remove"), { CMDNAME("gun"), CMDNAME("weapon") }))
		{
		}

		void createCommand(CommandList* list, Label&& menu_name, std::string&& config_name, std::vector<hash_t>&& weapons) final
		{
			std::vector<CommandName> command_names{};
			if (weapons.size() > 1)
			{
				command_names = getSuffixedCommandNames(CMDNAME("all"));
				command_names.insert(command_names.begin(), CMDNAME("noguns"));
				command_names.emplace_back(CMDNAME("noweapons"));
			}
			else
			{
				CommandName command_name = utf8ToCmdName(config_name);
				StringUtils::simplify(command_name);
				command_names = getSuffixedCommandNames(command_name);
			}
			list->createChild<Item>(std::move(menu_name), std::move(command_names), std::move(config_name), std::move(weapons));
		}

	private:
		class Item : public CommandAction
		{
		private:
			std::string config_name;
			std::vector<hash_t> weapons;

		public:
			explicit Item(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, std::string&& config_name, std::vector<hash_t>&& weapons)
				: CommandAction(parent, std::move(menu_name), std::move(command_names), NOLABEL, CMDFLAGS_ACTION, COMMANDPERM_RUDE), config_name(std::move(config_name)), weapons(std::move(weapons))
			{
			}

			[[nodiscard]] std::string getNameForConfig() const final
			{
				return config_name;
			}

			void onClick(Click& click) final
			{
				ensureScriptThread(click, [this](Click& click)
				{
					click.getEffectiveIssuer().getPed().removeWeapons(std::vector<hash_t>(weapons));
				});
			}
		};
	};
}
