#pragma once

#include <memory>
#include <vector>

#include "GridItem.hpp"
#include "Position2d.hpp"

namespace Stand
{
	using GridPredrawFlags_t = uint8_t;
#define GRID_UPDATE_POSITIONS		0b1
#define GRID_UPDATE_MENU_ADDRESSBAR	0b10

#pragma pack(push, 1)
	class Grid
	{
	public:
		Position2d origin;
		int16_t spacer_size;
		soup::SharedPtr<std::vector<std::unique_ptr<GridItem>>> items;
		int16_t border_width = 0;
		bool border_rounded = false;
		DirectX::SimpleMath::Color border_colour = { 1.0f, 0.0f, 1.0f, 1.0f };
		GridPredrawFlags_t predraw_flags = 0;
		bool content_update_queued = false;

	public:
		explicit Grid(Position2d origin, int16_t spacer_size);
		explicit Grid(int16_t origin_x, int16_t origin_y, int16_t spacer_size);
		virtual ~Grid() = default;

		void updateSetting(const std::function<void()>& impl);
		void updateSettingSimple(const std::function<void()>& impl);

		void update();
		void updateNow();
		void updatePositions();

	protected:
		virtual void populate(std::vector<std::unique_ptr<GridItem>>& items_draft) = 0;

	public:
		void setPositions(const std::vector<std::unique_ptr<GridItem>>& items, size_t recursions = 0) const;

		[[nodiscard]] static GridItem* getOccupant(const std::vector<std::unique_ptr<GridItem>>& items, int16_t x, int16_t y);
		[[nodiscard]] static GridItem* getOccupant(const std::vector<std::unique_ptr<GridItem>>& items, int16_t x, int16_t y, GridItem* exclude);
		[[nodiscard]] static GridItem* getOccupantNarrow(const std::vector<std::unique_ptr<GridItem>>& items, int16_t x, int16_t y, GridItem* stop_at);

		[[nodiscard]] GridItem* getItemByType(GridItemType target) const;
		[[nodiscard]] static GridItem* getItemByType(const std::vector<std::unique_ptr<GridItem>>& items, GridItemType target);

		void draw();

		void getBounds(int16_t& x1, int16_t& y1, int16_t& x2, int16_t& y2) const;
		void getDimensions(int16_t& x, int16_t& y, int16_t& width, int16_t& height) const;
	};
#pragma pack(pop)
}
