#include "CommandGarage.hpp"

#include "CommandSaveVehicle.hpp"
#include "CommandSavedVehicle.hpp"
#include "CommandSearchLiterals.hpp"

namespace Stand
{
	CommandGarage::CommandGarage(CommandList* parent)
		: CommandListFolder(parent, L"Vehicles", { L".txt" }, LOC("GRG"))
	{
		CommandIssuable::collapse_command_names.emplace(L"vehicle");
	}

	void CommandGarage::populateHead()
	{
		this->createChild<CommandSearchLiterals<true>>({ CMDNAME("findsaved"), CMDNAME("garagefind") });
		this->createChild<CommandSaveVehicle>();
		createOpenFolderAction();
	}

	CommandListFolderItemData* CommandGarage::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandSavedVehicle>(cmd);
	}

	void CommandGarage::onItemIsKept(Command* cmd) const
	{
		if (cmd->type != COMMAND_LIST)
		{
			((CommandSavedVehicle*)cmd)->data.data.clear();
		}
	}

	void CommandGarage::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandSavedVehicle>(std::move(filename_no_ext), std::move(data));
	}
}
