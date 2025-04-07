#include "CommandSavedPlace.hpp"

#include <fstream>

#include "AbstractEntity.hpp"
#include "CommandListPlaces.hpp"
#include "CommandListSavedPlaces.hpp"
#include "DrawUtil3d.hpp"
#include "parse_coord.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	CommandSavedPlace::CommandSavedPlace(CommandList* parent, std::wstring&& place_name, CommandListFolderItemData&& folder_item_data)
		: CommandWithOnTickFocused(parent, LIT(std::move(place_name)), CMDNAMES(), NOLABEL, CMDFLAGS_ACTION_SCRIPT | CMDFLAG_FEATURELIST_SKIP), folder_item_data(std::move(folder_item_data))
	{
		CommandName simple_name = menu_name.getLiteralForCommandName();
		StringUtils::simplify(simple_name);
		command_names = combineCommandNames(getCommandListFolder()->parent->command_names, simple_name);
	}

	bool CommandSavedPlace::ensureCoord()
	{
		if (!data.has_value())
		{
			std::ifstream file_in(getFilePath());
			std::string line;
			if (!std::getline(file_in, line))
			{
				return false;
			}
			auto coord = parse_coord(line);
			if (!coord.has_value())
			{
				return false;
			}
			data = std::move(coord);
		}
		return true;
	}

	void CommandSavedPlace::onTickFocused()
	{
		if (ensureCoord())
		{
			DrawUtil3d::draw_line_and_ar_beacon(data.value());
		}
	}

	void CommandSavedPlace::onClickScriptThread(Click& click)
	{
		auto* list = getCommandListFolder()->as<CommandListSavedPlaces>();
		if (!ensureCoord())
		{
			list->refresh();
		}
		else
		{
			PointOfPresence pop;
			pop.pos = data.value();
			list->parent->as<CommandListPlaces>()->processPop(click, pop);
		}
	}
}
