#pragma once

#include "CommandPosition3d.hpp"

#include "AbstractEntity.hpp"
#include "vector.hpp"

namespace Stand
{
	class CommandEntityPosition : public CommandPosition3d
	{
	private:
		AbstractEntity ent;

	public:
		explicit CommandEntityPosition(CommandList* parent, std::vector<CommandName>&& command_names, AbstractEntity ent)
			: CommandPosition3d(parent, LOC("POS"), std::move(command_names)), ent(ent)
		{
			populate();
		}

		[[nodiscard]] v3 getVec() final
		{
			return ent.getPos();
		}

		void setVec(v3&& pos) final
		{
			ent.setPos(std::move(pos));
		}
	};
}
