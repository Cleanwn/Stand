#include "CommandProfiles.hpp"

#include "CommandProfilesCreate.hpp"
#include "CommandProfile.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "Script.hpp"

namespace Stand
{
	CommandProfiles::CommandProfiles(CommandList* const parent)
		: CommandListFolder(parent, L"Profiles", { L".txt" }, LOC("PRFLS"), { CMDNAME("profiles") }, LOC("PRFLS_H"))
	{
		CommandPhysical::collapse_command_names.emplace(L"save");
		CommandPhysical::collapse_command_names.emplace(L"saveprofile");
		CommandPhysical::collapse_command_names.emplace(L"load");
		CommandPhysical::collapse_command_names.emplace(L"loadprofile");
		CommandPhysical::collapse_command_names.emplace(L"autosave");
		CommandPhysical::collapse_command_names.emplace(L"autosaveprofile");
	}

	void CommandProfiles::onClick(Click& click)
	{
		if (g_gui.profiles_tutorial == 3)
		{
			FiberPool::queueJob([]
			{
				Script::current()->yield();
				g_gui.setProfilesTutorialDone();
			});
		}

		CommandListFolder::onClick(click);
	}

	void CommandProfiles::populateHead()
	{
		this->createChild<CommandProfilesCreate>();
		createOpenFolderAction();
	}

	CommandListFolderItemData* CommandProfiles::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandProfile>(cmd);
	}

	void CommandProfiles::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandProfile>(std::move(filename_no_ext), std::move(data));
	}
}
