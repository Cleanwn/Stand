#pragma once

#include "CommandBirenderPlayerEsp.hpp"

#include "AbstractEntity.hpp"
#include "Box.hpp"

namespace Stand
{
	class CommandPlayerBoxEsp : public CommandBirenderPlayerEsp
	{
	public:
		explicit CommandPlayerBoxEsp(CommandList* const parent, CommandSlider* const max_distance, CommandColour* const colour, CommandEspTags* const tag_colours)
			: CommandBirenderPlayerEsp(parent, LOC("BOXESP"), { CMDNAME("boxesp"), CMDNAME("playerboxesp") }, max_distance, colour, tag_colours)
		{
		}

	protected:
		void drawEsp(const AbstractRenderer& renderer, AbstractPlayer p, const v3& pos, float dist, DirectX::SimpleMath::Color& colour) const final
		{
			if (auto ped = p.getPed(); ped.isValid())
			{
				auto box = ped.getBox();

				box.edge2.z += 0.3f;
				box.edge7.z += 0.3f;
				box.edge8.z += 0.3f;
				box.back_lower_left.z += 0.3f;

				box.drawLines(renderer, colour);
			}
		}
	};
}
