#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandPlayerBoneEsp : public CommandBoneEsp
	{
	public:
		explicit CommandPlayerBoneEsp(CommandList* const parent, CommandColour* const colour, CommandEspTags* const tag_colours = nullptr)
			: CommandBoneEsp(parent, CMDNAMES("playerboneesp", "playerbonesp"), colour, tag_colours)
		{
		}

	protected:
		bool filterPed(AbstractEntity& ped) const final
		{
			return ped.isAPlayer() && ped != g_player_ped;
		}
	};
}
