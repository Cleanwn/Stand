#pragma once

#include "CommandListCommandBoxPresets.hpp"
#include "Exceptional.hpp"
#include "get_appdata_path.hpp"
#include "Gui.hpp"

namespace Stand
{
	CommandSaveCommandBoxPreset::CommandSaveCommandBoxPreset(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::wstring&& folder_name)
		: CommandActionName(parent, std::move(menu_name), std::move(command_names), std::move(help_text)), folder_name(folder_name)
	{
	}

	void CommandSaveCommandBoxPreset::onClick(Click& click)
	{
		std::wstring prefill{ cmdNameToUtf16(command_names.at(0)) };
		prefill.push_back(L' ');
		__try
		{
			std::filesystem::path path = get_appdata_path(fmt::format(L"Scripts\\{}\\", folder_name));
			if (!std::filesystem::exists(path))
			{
				std::filesystem::create_directory(path);
			}
			prefill.append(folder_name + L"\\");
		}
		__EXCEPTIONAL()
		{
		}
		click.showCommandBoxIfPossible(std::move(prefill));
	}

	void CommandSaveCommandBoxPreset::onCommand(Click& click, std::wstring& args)
	{
		if (!preCheck(click))
		{
			args.clear();
			return;
		}

		std::ofstream out(get_appdata_path(fmt::format(L"Scripts\\{}.txt", args)));
		args.clear();

		if (out.fail())
		{
			click.setResponse(LOC("BADFILE"));
			g_gui.showCommandBox(std::move(cmdNameToUtf16(command_names.at(0)).append(args)), click);
			return;
		}

		if (performSave(click, out))
		{
			this->parent->as<CommandListCommandBoxPresets<CommandList>>()->refreshScriptsLink();
		}
	}
}
