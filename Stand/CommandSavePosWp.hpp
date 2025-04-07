#pragma once

#include "CommandSavePos.hpp"

#include "Gui.hpp"

namespace Stand
{
	class CommandSavePosWp : public CommandSavePos
	{
	public:
		explicit CommandSavePosWp(CommandList* parent)
			: CommandSavePos(parent, LOC("SVPLC_W"), CMDNAMES("savewppos", "savewpcoords", "savewaypointpos", "savewaypointcoords"))
		{
		}

		void onClick(Click& click) final
		{
			if (g_gui.waypoint.isNull())
			{
				click.setResponse(LOC("BLIPNFND"));
				return;
			}
			CommandSavePos::onClick(click);
		}

		void onCommand(Click& click, std::wstring& args) final
		{
			if (g_gui.waypoint.isNull())
			{
				click.setResponse(LOC("BLIPNFND"));
				return;
			}
			CommandSavePos::onCommand(click, args);
		}

		[[nodiscard]] std::string getPos() const final
		{
			return g_gui.waypoint.operator std::string();
		}
	};
}
