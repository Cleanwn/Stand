#pragma once

#include <functional>

#include "Grid.hpp"
#include "TextSettings.hpp"

namespace Stand
{
	enum class ScrollbarMode : uint8_t
	{
		DISABLED,
		ENABLED_WHEN_NEEDED,
		ENABLED
	};

#pragma pack(push, 1)
	class MenuGrid : public Grid
	{
	public:
		static constexpr Position2d default_origin = { 1323, 560 };

		bool list_update_queued = false;

		uint8_t root_cursor = 0;
		GridItem* main_view;
		std::wstring untrimmed_menu_name{};

		int16_t info_padding = 0;
		int16_t info_width = 300;
		ScrollbarMode scrollbar_mode = ScrollbarMode::ENABLED_WHEN_NEEDED;
		int16_t scrollbar_width = 6;
		bool left_space_before_all_commands = false;
		bool leftbound_textures_toggles = false;
		bool leftbound_textures_nontoggles = false;
		bool number_sliders_rightbound_values = true;

		bool addressbar_width_affected_by_scrollbar = false;
		bool addressbar_width_affected_by_columns = false;
		TextSettings addressbar_text{ float(12.0 * TEXT_HEIGHT_PX * 2.0), -2.0f, 2.0f };

		std::string qrcode{};

		explicit MenuGrid();

		[[nodiscard]] GridItemList* getColumn(const Command* target) const noexcept;

		void updateAddress();
		void updateList(const Command* target);

		void doUpdateAddress(const std::vector<std::unique_ptr<GridItem>>& items);

	public:
		void populate(std::vector<std::unique_ptr<GridItem>>& items_draft) final;
	private:
		void populateHeader(std::vector<std::unique_ptr<GridItem>>& items_draft, int16_t full_menu_width, uint8_t priority, GridItem* force_alignment_to);
	};
#pragma pack(pop)

	inline MenuGrid g_menu_grid = MenuGrid();
}
