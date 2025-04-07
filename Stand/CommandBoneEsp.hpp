#pragma once

#include "CommandBirender.hpp"

#include "AbstractEntity.hpp"

namespace Stand
{
	class CommandBoneEsp : public CommandBirender
	{
	protected:
		std::vector<AbstractEntity> peds = {};

	public:
		explicit CommandBoneEsp(CommandList* parent, std::vector<CommandName>&& command_names, CommandColour* const colour, CommandEspTags* const tag_colours = nullptr)
			: CommandBirender(parent, LOC("BONESP"), std::move(command_names), colour, tag_colours)
		{
		}

	protected:
		void onTick() final;
		void draw(const AbstractRenderer& renderer) final;

		virtual bool filterPed(AbstractEntity& ped) const = 0;
	};
}
