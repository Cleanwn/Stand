#pragma once

#include "CommandList.hpp"

#include "CommandRedirectToList.hpp"
#include "CommandSaveCommandBoxPreset.hpp"
#include "CommandScripts.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	template <class T>
	class CommandListCommandBoxPresets : public CommandList
	{
	private:
		Label view_menu_name;
		std::string folder_name;
		CommandPhysical* redirect = nullptr;

	public:
		explicit CommandListCommandBoxPresets(CommandList* const parent, std::vector<CommandName>&& command_names, std::string&& folder_name, Label&& menu_name = LOC("PRSTS"), Label&& save_menu_name = LOC("SVPRST"), Label&& view_menu_name = LOC("VWPRSTS"), Label&& save_help_text = NOLABEL)
			: CommandList(parent, std::move(menu_name), std::move(command_names)), folder_name(folder_name), view_menu_name(std::move(view_menu_name))
		{
			createChild<T>(std::move(save_menu_name), combineCommandNames(this->command_names, CMDNAMES_OBF("save")), std::move(save_help_text), StringUtils::utf8_to_utf16(this->folder_name));
		}

		void refreshScriptsLink()
		{
			CommandScripts::instance->refresh([this]
			{
				CommandList* scripts_folder = CommandScripts::instance->findFolder(folder_name);
				if (scripts_folder == nullptr)
				{
					if (redirect != nullptr)
					{
						redirect->beGone();
						redirect = nullptr;
					}
				}
				else
				{
					if (redirect == nullptr)
					{
						auto copy = view_menu_name;
						auto link = this->createChild<CommandRedirectToList>(scripts_folder, std::move(copy));
						redirect = link;
						processChildrenUpdate();
					}
				}
			});
		}

		void onActiveListUpdate()
		{
			refreshScriptsLink();

			return CommandList::onActiveListUpdate();
		}
	};
}