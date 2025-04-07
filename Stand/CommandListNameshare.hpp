#pragma once

#include "CommandList.hpp"

namespace Stand
{
	class CommandListNameshare : public CommandList
	{
	public:
		explicit CommandListNameshare(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names = {}, Label&& help_text = NOLABEL, const commandflags_t flags = CMDFLAGS_LIST, const CommandType type = COMMAND_LIST_NAMESHARE);

	private:
		[[nodiscard]] std::vector<CommandPhysical*> processArgs(CommandPerm perms, std::wstring& args);

	public:
		void onCommand(Click& click, std::wstring& args) override;
		void getExtraInfo(CommandExtraInfo& info, std::wstring& args) final;
	};
}
