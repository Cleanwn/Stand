#pragma once

#include "CommandSavePos.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandSavePosMe : public CommandSavePos
	{
	public:
		explicit CommandSavePosMe(CommandList* parent)
			: CommandSavePos(parent, LOC("SVPLC_M"), { CMDNAME("savepos"), CMDNAME("savecoords"), CMDNAME("saveplace") })
		{
		}

		[[nodiscard]] std::string getPos() const final
		{
			return g_player_ent.getPos().operator std::string();
		}
	};
}
