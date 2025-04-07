#pragma once

#include "CommandToggle.hpp"

#include "AbstractEntity.hpp"
#include "CommandAestheticLightPlacement.hpp"
#include "CommandColour.hpp"
#include "CommandSlider.hpp"
#include "natives.hpp"

namespace Stand
{
	class CommandAestheticLight : public CommandToggle
	{
	private:
		CommandColour* const colour;
		CommandAestheticLightPlacement* const placement;
		CommandSlider* const range;
		CommandSlider* const intensity;
		CommandSlider* const shadow;

	public:
		explicit CommandAestheticLight(CommandList* const parent, CommandColour* colour, CommandAestheticLightPlacement* placement, CommandSlider* range, CommandSlider* intensity, CommandSlider* shadow)
			: CommandToggle(parent, LOC("AESLGHT")), colour(colour), placement(placement), range(range), intensity(intensity), shadow(shadow)
		{
		}

		void onEnable(Click& click) final
		{
			registerScriptTickEventHandler(click, [this]()
			{
				if (m_on)
				{
					const DirectX::SimpleMath::Vector3 rgb = colour->getRGB();
					switch (placement->value)
					{
					case 0:
						drawLight(rgb, g_player_ped.getVisualPos());
						break;

					case 1:
						drawLight(rgb, CAM::GET_FINAL_RENDERED_CAM_COORD());
						break;

					case 2:
						const v3 ped_pos = g_player_ped.getVisualPos();
						const float scale_multiplier = g_player_ped.getDimensions().z * 0.5f;
						for (float pitch = 0.0f; pitch < 360.0f; pitch += 60.0f)
						{
							const v3 pitch_pos = ped_pos + v3(0.0f, 0.0f, v3(pitch, 0.0f, 0.0f).toDirZ() * scale_multiplier);
							for (float heading = 0.0f; heading < 360.0f; heading += 60.0f)
							{
								drawLight(rgb, pitch_pos + (v3(0.0f, 0.0f, heading).toDirNoZ() * (scale_multiplier * 0.5f))); // Assumes height = depth + width
							}
						}
						break;
					}
				}
				return m_on;
			});
		}

	private:
		void drawLight(const DirectX::SimpleMath::Vector3& rgb, const v3& pos) const
		{
			GRAPHICS::DRAW_LIGHT_WITH_RANGEEX(pos.x, pos.y, pos.z, (int)(rgb.x * 255.0f), (int)(rgb.y * 255.0f), (int)(rgb.z * 255.0f), (float)range->value / 100.0f, (float)intensity->value / 100.0f, (float)shadow->value / 100.0f);
		}
	};
}
