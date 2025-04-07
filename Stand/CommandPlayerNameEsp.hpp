#pragma once

#include "CommandBirenderPlayerEsp.hpp"

#include "AbstractRenderer.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	class CommandPlayerNameEsp : public CommandBirenderPlayerEsp
	{
	private:
		CommandToggleNoCorrelation* const show_tags;
		CommandSlider* const min_text_scale;
		CommandSlider* const max_text_scale;
		CommandToggleNoCorrelation* const invert_text_scale;

	public:
		explicit CommandPlayerNameEsp(CommandList* const parent, CommandSlider* const max_distance, CommandToggleNoCorrelation* const show_tags, CommandSlider* const min_text_scale, CommandSlider* const  max_text_scale, CommandToggleNoCorrelation* const invert_text_scale, CommandColour* const colour, CommandEspTags* const tag_colours)
			: CommandBirenderPlayerEsp(parent, LOC("NAMESP"), CMDNAMES("nameesp", "namesp", "playernameesp", "playernamesp"), max_distance, colour, tag_colours), show_tags(show_tags), min_text_scale(min_text_scale), max_text_scale(max_text_scale), invert_text_scale(invert_text_scale)
		{
		}

	protected:
		void drawEsp(const AbstractRenderer& renderer, AbstractPlayer p, const v3& pos, float dist, DirectX::SimpleMath::Color& colour) const final
		{
			const auto max_distance = (float)this->max_distance->value;
			const float min_text_scale = (float)this->min_text_scale->value / 100.0f;
			const float max_text_scale = (float)this->max_text_scale->value / 100.0f;
			const float mul_text_scale = (max_text_scale - min_text_scale);
			float text_scale = ((mul_text_scale / max_distance) * dist);
			if (invert_text_scale->m_on)
			{
				text_scale = ((mul_text_scale + 1.0f) / (text_scale + 1.0f)) - 1.0f;
			}
			renderer.drawText(pos, p.getNameW(show_tags->m_on), text_scale + min_text_scale, colour);
		}
	};
}
