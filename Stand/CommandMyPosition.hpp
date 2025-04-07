#pragma once

#include "CommandPosition3d.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandMyPosition : public CommandPosition3d
	{
	public:
		explicit CommandMyPosition(CommandList* parent)
			: CommandPosition3d(parent, LOC("MYPOS"), CMDNAMES("pos", "coords"))
		{
		}

		[[nodiscard]] v3 getVec() final
		{
			return g_player_ent.getPos();
		}

		void setVec(v3&& pos) final
		{
			g_player_ent.setPos(std::move(pos));
		}
	};
}
