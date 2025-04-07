#pragma once

#include "CommandActionName.hpp"

#include "ConfigState.hpp"
#include "Exceptional.hpp"
#include "Gui.hpp"

namespace Stand
{
	class CommandProfilesCreate : public CommandActionName
	{
	public:
		explicit CommandProfilesCreate(CommandList* const parent)
			: CommandActionName(parent, LOC("NEWPRFL"), { CMDNAME("newprofile"), CMDNAME("createprofile") }, LOC("NEWPRFL_H"), CMDFLAGS_ACTION_NAME | CMDFLAG_STATE_AND_CHECK_FINISHLIST)
		{
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (!onCommandValidateFileName(click, args))
			{
				return;
			}
			click.setNoResponse();
			Exceptional::createManagedExceptionalThread(__FUNCTION__, [this, args{ std::move(args) }]() mutable
			{
				std::wstring name = L"Profiles\\";
				name.append(args);
				ConfigState config(std::move(name));
				g_gui.saveStateInMemory(config.data);
				config.saveToFileNow();
				parent->as<CommandListRefreshable>()->refresh();
			});
			args.clear();
		}
	};
}
