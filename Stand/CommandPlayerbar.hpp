#pragma once

#include "CommandList.hpp"

#include "dx_common.hpp"
#include "player_tags.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class CommandPlayerbar : public CommandList
	{
	public:
		bool tag_provides_colour[FLAG_COUNT]{};
		DirectX::SimpleMath::Color tag_colours[FLAG_COUNT + 1];

		float text_scale;
		CommandSlider* bgblur;
		CommandListSelect* hide_if;
		CommandToggleNoCorrelation* show_if_alone;
		CommandToggleNoCorrelation* show_tags;
		CommandToggleNoCorrelation* hide_no_tags;

		explicit CommandPlayerbar(CommandList* const parent);
	private:
		void setDefaultTagColour(playerflag_t id, DirectX::SimpleMath::Color&& colour);
		void createTagColourList(playerflag_t id, Label&& name);
	};
#pragma pack(pop)
}
