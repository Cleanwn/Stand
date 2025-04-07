#include "CommandLuaScripts.hpp"

#include <filesystem>

#include "CommandListSelect.hpp"
#include "CommandLuaConfigToggle.hpp"
#include "CommandSlider.hpp"
#include "LuaConfig.hpp"
#include "PackageMgr.hpp"

#include "CommandLuaConfigPresets.hpp"
#include "CommandLuaConfigEnforceSilentStop.hpp"

#include "CommandLuaScript.hpp"
#include "CommandPackages.hpp"

namespace Stand
{
	CommandLuaScripts::CommandLuaScripts(CommandList* parent)
		: CommandListFolder(parent, L"Lua Scripts", { L".lua", L".pluto" }, LOC("LUA"), { CMDNAME("luascripts") }, LOC("LUA_H"))
	{
		CommandPhysical::collapse_command_names.emplace(L"lua");
		CommandPhysical::collapse_command_names.emplace(L"startlua");
		CommandPhysical::collapse_command_names.emplace(L"stoplua");
	}

	void CommandLuaScripts::onClick(Click& click)
	{
		if (!PackageMgr::updating_completed)
		{
			// User might peak in during update where an installed script file was deleted, so the state would not be loaded.
			click.setResponse(LOC("GENWAIT"));
			return;
		}
		CommandListFolder::onClick(click);
	}

	void CommandLuaScripts::populateHead()
	{
		createChild<CommandPackages>();
		createOpenFolderAction();

		{
			auto settings = createChild<CommandList>(LOC("STTNGS"));

			settings->createChild<CommandLuaConfigPresets>();

			LuaConfig::warn_dangerous = settings->createChild<CommandLuaConfigToggle>(LOC("WRNOS"), CMDNAMES("luaconfigwarndangerous"), NOLABEL, true);
			LuaConfig::warn_money = settings->createChild<CommandLuaConfigToggle>(LOC("WRNMNY"), CMDNAMES("luaconfigwarnmoney"), NOLABEL, true);

			LuaConfig::enforce_silent_start = settings->createChild<CommandListSelect>(LOC("ENFRCSILNTSTRT"), {}, NOLABEL, std::vector<CommandListActionItemData>{
				{0, LOC("NO")},
				{1, LOC("YES")},
				{2, LOC("YES_NOTIFY")},
			}, 1);
			LuaConfig::enforce_silent_stop = settings->createChild<CommandLuaConfigEnforceSilentStop>();

			settings->createChild<CommandDivider>(LOC("4DEV"));

			LuaConfig::enable_warnings = settings->createChild<CommandLuaConfigToggle>(LIT("enable_warnings"));
			LuaConfig::notify_deprecated = settings->createChild<CommandLuaConfigToggle>(LIT("notify_deprecated"));
			LuaConfig::notify_bad_practice = settings->createChild<CommandLuaConfigToggle>(LIT("notify_bad_practice"));
			LuaConfig::notify_missed_cleanup_blurrect = settings->createChild<CommandLuaConfigToggle>(LIT("notify_missed_cleanup_blurrect"));
			LuaConfig::notify_missed_cleanup_arspinner = settings->createChild<CommandLuaConfigToggle>(LIT("notify_missed_cleanup_arspinner"));
			LuaConfig::notify_missed_cleanup_graceland = settings->createChild<CommandLuaConfigToggle>(LIT("notify_missed_cleanup_graceland"));
			LuaConfig::notify_ref_by_path_upgrade = settings->createChild<CommandLuaConfigToggle>(LIT("notify_ref_by_path_upgrade"));
			LuaConfig::notify_get_value_fail = settings->createChild<CommandLuaConfigToggle>(LIT("notify_get_value_fail"), {}, LIT("If menu.get_value does not support a command type, it will silently return 0. With this enabled, you will get a notification in that case."));
		}
	}

	CommandListFolderItemData* CommandLuaScripts::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandLuaScript>(cmd);
	}

	bool CommandLuaScripts::canShowFolder(const std::wstring& folder_rel_path) const
	{
		return folder_rel_path != L"lib"
			&& folder_rel_path != L"store"
			&& folder_rel_path != L"resources"
			&& folder_rel_path != L"bin"
			;
	}

	void CommandLuaScripts::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandLuaScript>(std::move(filename_no_ext), std::move(data));
	}
}
