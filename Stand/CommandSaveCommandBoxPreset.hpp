#pragma once

#include "CommandActionName.hpp"

#include <fstream>

namespace Stand
{
	class CommandSaveCommandBoxPreset : public CommandActionName
	{
	private:
		std::wstring folder_name;

	public:
		explicit CommandSaveCommandBoxPreset(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, Label&& help_text, std::wstring&& folder_name);

		void onClick(Click& click) final;
		void onCommand(Click& click, std::wstring& args) final;

	protected:
		virtual bool preCheck(Click& click)
		{
			return true;
		}

		virtual bool performSave(Click& click, std::ofstream& out) const = 0;
	};
}
