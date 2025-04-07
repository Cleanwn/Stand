#include "CommandScripts.hpp"

#include <soup/ObfusString.hpp>

#include "CommandReadonlyLink.hpp"
#include "CommandScript.hpp"
#include "FileLogger.hpp"

namespace Stand
{
	inline std::unordered_map<std::wstring, UserScript> preset_scripts = {
		{L"Fast Alt+F4", {"Makes Alt+F4 close the game instantly", {
			"addhotkey yeet Alt+F4",
			"hotkeysskipwarnings on",
		}}},
		{L"Graphics Mod", {"Some rendering changes to make the game look... different", {
			"shader intnofog",
			"lodscale 3.00",
		}}},
	};

	CommandScripts::CommandScripts(CommandList* parent)
		: CommandListFolder(parent, L"Scripts", { L".txt" }, LOC("CMDSCRPT"))
	{
		instance = this;

		CommandPhysical::collapse_command_names.emplace(L"exec");
	}

	CommandScripts::~CommandScripts()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}

	void CommandScripts::populateHead()
	{
		createOpenFolderAction();

		createChild<CommandReadonlyLink>(LOC("CMDBXDCS"), "https://commands.chat/");
		createChild<CommandReadonlyLink>(LOC("CMDLST"), soup::ObfusString("https://stand.sh/commands"));
	}

	void CommandScripts::postPopulateBody() const
	{
		const auto listed_items = getListedItems();
		for (const auto& listed_item : listed_items)
		{
			auto listed_item_no_ext = listed_item.substr(0, listed_item.length() - 4);
			if (preset_scripts.find(listed_item_no_ext) == preset_scripts.end())
			{
				return;
			}
		}
		if (listed_items.size() != preset_scripts.size())
		{
			for (const auto& preset_script : preset_scripts)
			{
				std::ofstream file_out(getFolderPath().append(preset_script.first).append(L".txt"));
				file_out << "Description: " << preset_script.second.description << "\n\n";
				for (const auto& command : preset_script.second.commands)
				{
					file_out << command << "\n";
				}
			}
		}
	}

	CommandListFolderItemData* CommandScripts::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandScript>(cmd);
	}

	void CommandScripts::onItemIsKept(Command* cmd) const
	{
		if (cmd->type != COMMAND_LIST)
		{
			((CommandScript*)cmd)->data = std::nullopt;
		}
	}

	void CommandScripts::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandScript>(std::move(filename_no_ext), std::move(data));
	}
}
