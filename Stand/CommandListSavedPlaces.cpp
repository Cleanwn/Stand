#include "CommandListSavedPlaces.hpp"

#include "CommandSavedPlace.hpp"
#include "lang.hpp"

namespace Stand
{
	[[nodiscard]] static Label getHelpText()
	{
		std::string path = LANG_GET("WRLD");
		path.append(" > ");
		path.append(LANG_GET("PLCS"));
		path.append(" > ");
		path.append(LANG_GET("POS"));
		return LIT(LANG_FMT("SVDPLCS_H", path));
	}

	CommandListSavedPlaces::CommandListSavedPlaces(CommandList* parent)
		: CommandListFolder(parent, L"Places", { L".txt" }, LOC("SVDPLCS"), {}, getHelpText())
	{
	}

	CommandListFolderItemData* CommandListSavedPlaces::getItemData(Command* cmd) const noexcept
	{
		return getItemDataImpl<CommandSavedPlace>(cmd);
	}

	void CommandListSavedPlaces::onItemIsKept(Command* cmd) const
	{
		((CommandSavedPlace*)cmd)->data.reset();
	}

	void CommandListSavedPlaces::createItem(CommandList* list, std::wstring&& filename_no_ext, CommandListFolderItemData&& data) const
	{
		list->createChild<CommandSavedPlace>(std::move(filename_no_ext), std::move(data));
	}
}
