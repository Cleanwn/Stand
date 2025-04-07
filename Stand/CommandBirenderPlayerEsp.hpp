#pragma once

#include "CommandBirender.hpp"

#include "AbstractPlayer.hpp"
#include "CommandSlider.hpp"
#include "natives.hpp"
#include "vector.hpp"

namespace Stand
{
	class CommandBirenderPlayerEsp : public CommandBirender
	{
	protected:
		struct RenderInfo
		{
			AbstractPlayer p;
			v3 pos;
			float dist;
			DirectX::SimpleMath::Color colour;
		};

		std::vector<RenderInfo> players{};
		CommandSlider* const max_distance;

	public:
		explicit CommandBirenderPlayerEsp(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, CommandSlider* max_distance, CommandColour* const colour, CommandEspTags* const tag_colours)
			: CommandBirender(parent, std::move(menu_name), std::move(command_names), colour, tag_colours), max_distance(max_distance)
		{
		}

	protected:
		void onTick() final
		{
			players.clear();

			const auto cam_pos = CAM::GET_FINAL_RENDERED_CAM_COORD();
			const auto max_distance = (float)this->max_distance->value;
			for (AbstractPlayer p : AbstractPlayer::listExcept(true))
			{
				v3 pos = p.getPos();
				if (!pos.isNull())
				{
					float dist = pos.distance(cam_pos);
					if (dist <= max_distance)
					{
						players.emplace_back(RenderInfo{ p, std::move(pos), dist, getColour(p) });
					}
				}
			}
		}

		void draw(const AbstractRenderer& renderer) final
		{
			for (auto& ri : players)
			{
				drawEsp(renderer, ri.p, ri.pos, ri.dist, ri.colour);
			}
		}

		virtual void drawEsp(const AbstractRenderer& renderer, AbstractPlayer p, const v3& pos, float dist, DirectX::SimpleMath::Color& colour) const = 0;
	};
}
