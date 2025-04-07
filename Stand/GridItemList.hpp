#pragma once

#include "GridItem.hpp"

#include <string>
#include <vector>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "Texture.hpp"
#include "TextureDynamic.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct DrawCommandData
	{
		DirectX::SimpleMath::Color textColour;
		DirectX::SimpleMath::Color rightTextColour;
		bool focused = false;
		bool hasRectColourOverride = false;
		DirectX::SimpleMath::Color rectColourOverride;
		Texture* leftbound_texture = nullptr;
		Texture* rightbound_texture = nullptr;
		DirectX::SimpleMath::Color texture_colour;
		std::wstring rightbound_text{};
		float rightbound_text_width;
		std::wstring trimmed_name{};
		bool centered = false;

		void setSlider(std::wstring&& value, std::wstring& name, LONG& command_name_max_width);
	};

	struct DrawListData
	{
		cursor_t offset;
		bool has_extra_top = false;
		std::vector<DrawCommandData> list{};
	};

	class GridItemList : public GridItem
	{
	public:
		CommandList* const view;
		soup::SharedPtr<DrawListData> draw_data;
		const cursor_t offset;
		TextureDynamic contentsTex{};

		explicit GridItemList(CommandList* view, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, cursor_t offset);
	public:
		void update();
	private:
		static LONG drawSpriteCommandRight(DrawCommandData& draw_data, Texture* texture, const DirectX::SimpleMath::Color& colour, bool leftbound);

	public:
		[[nodiscard]] bool containsCommand(const Command* target) const noexcept;

		void draw() final;
	};
#pragma pack(pop)
}
