#include "Grid.hpp"

#include <algorithm>

#include "GridItem.hpp"
#include "Gui.hpp"
#include "MenuGrid.hpp"
#include "Renderer.hpp"
#include "Worker.hpp"

#define spacer_x spacer_size
#define spacer_y spacer_size

namespace Stand
{
	Grid::Grid(Position2d origin, int16_t spacer_size)
		: origin(std::move(origin)), spacer_size(spacer_size), items(soup::make_shared<std::vector<std::unique_ptr<GridItem>>>())
	{
	}

	Grid::Grid(int16_t origin_x, int16_t origin_y, int16_t spacer_size)
		: Grid(Position2d{ origin_x, origin_y }, spacer_size)
	{
	}

	void Grid::updateSetting(const std::function<void()>& impl)
	{
		if (g_gui.canUpdateThemeSetting())
		{
			impl();
			update();
		}
	}

	void Grid::updateSettingSimple(const std::function<void()>& impl)
	{
		if (g_gui.canUpdateThemeSetting())
		{
			impl();
			updatePositions();
		}
	}

	void Grid::update()
	{
		if (!content_update_queued)
		{
			content_update_queued = true;
			Worker::queueJob([this]
			{
				content_update_queued = false;
				updateNow();
			});
		}
	}

	void Grid::updateNow()
	{
		auto items_draft = soup::make_shared<std::vector<std::unique_ptr<GridItem>>>();
		populate(*items_draft);
		std::sort(items_draft->begin(), items_draft->end(), [](const std::unique_ptr<GridItem>& a, const std::unique_ptr<GridItem>& b)
		{
			return a->priority < b->priority;
		});
		setPositions(*items_draft);
		items = std::move(items_draft);
		predraw_flags = 0;
	}

	void Grid::updatePositions()
	{
		predraw_flags |= GRID_UPDATE_POSITIONS;
	}

	void Grid::setPositions(const std::vector<std::unique_ptr<GridItem>>& items, size_t recursions) const
	{
		GridItem* last_item = nullptr;
		{
			auto x = origin.x;
			auto y = origin.y;
			for (auto& item : items)
			{
				if (item->keep_pos)
				{
					item->keep_pos = false;
					x = item->x;
					y = item->y;
				}
				else
				{
					if (item->force_alignment_to != nullptr)
					{
						last_item = item->force_alignment_to;
						x = last_item->x;
						y = last_item->y;
					}
					if (last_item == nullptr)
					{
						switch (item->alignment_relative_to_last)
						{
						case ALIGN_TOP_CENTRE:
							y -= item->height;
							break;
						}
					}
					else
					{
						switch (item->alignment_relative_to_last)
						{
						case ALIGN_BOTTOM_LEFT:
							y += (last_item->height + spacer_y);
							break;

						case ALIGN_TOP_RIGHT:
							x += (last_item->width + spacer_x);
							break;

						case ALIGN_TOP_LEFT:
							x -= (item->width + spacer_x);
							if (auto* conflict_item = getOccupantNarrow(items, x + item->width - 1, y + 1, item.get()))
							{
								y += (conflict_item->height + spacer_y);
							}
							break;

						case ALIGN_BOTTOM_RIGHT:
							x -= (item->width - last_item->width);
							break;

						case ALIGN_TOP_CENTRE:
							y -= (item->height + spacer_y);
							break;
						}
					}
					item->x = x;
					item->y = y;
					item->onPositioned();
				}
				last_item = item.get();
			}
		}
		last_item = nullptr;
		bool need_redo = false;
		for (auto& item : items)
		{
			if (item->force_alignment_to != nullptr)
			{
				last_item = item->force_alignment_to;
			}
			switch (item->alignment_relative_to_last)
			{
			case ALIGN_BOTTOM_LEFT:
				if (auto* conflict_item = getOccupant(items, item->x + item->width - 1, item->y + 1, item.get()))
				{
					item->x -= ((item->x + item->width) - conflict_item->x) + spacer_x;
					item->keep_pos = true;
					need_redo = true;
				}
				break;
			}
			last_item = item.get();
		}
		if (need_redo && ++recursions <= items.size())
		{
			setPositions(items, recursions);
		}
	}

	GridItem* Grid::getOccupant(const std::vector<std::unique_ptr<GridItem>>& items, int16_t x, int16_t y)
	{
		for (const auto& item : items)
		{
			if (item->occupies(x, y))
			{
				return item.get();
			}
		}
		return nullptr;
	}

	GridItem* Grid::getOccupant(const std::vector<std::unique_ptr<GridItem>>& items, int16_t x, int16_t y, GridItem* exclude)
	{
		for (const auto& item : items)
		{
			if (item.get() != exclude && item->occupies(x, y))
			{
				return item.get();
			}
		}
		return nullptr;
	}

	GridItem* Grid::getOccupantNarrow(const std::vector<std::unique_ptr<GridItem>>& items, int16_t x, int16_t y, GridItem* stop_at)
	{
		for (const auto& item : items)
		{
			if (item.get() == stop_at)
			{
				break;
			}
			if (item->occupies(x, y))
			{
				return item.get();
			}
		}
		return nullptr;
	}

	GridItem* Grid::getItemByType(GridItemType target) const
	{
		auto items_capture{ items };
		return getItemByType(*items_capture, target);
	}

	GridItem* Grid::getItemByType(const std::vector<std::unique_ptr<GridItem>>& items, GridItemType target)
	{
		for (const auto& item : items)
		{
			if (item->type == target)
			{
				return item.get();
			}
		}
		return nullptr;
	}

	void Grid::draw()
	{
		auto items_capture{ items };
		if (predraw_flags != 0)
		{
			if (predraw_flags & GRID_UPDATE_POSITIONS)
			{
				setPositions(*items_capture);
			}
			if (predraw_flags & GRID_UPDATE_MENU_ADDRESSBAR)
			{
				((MenuGrid*)this)->doUpdateAddress(*items_capture);
			}
			predraw_flags = 0;
		}

		if (border_width != 0)
		{
			for (const auto& item : *items_capture)
			{
				g_renderer.drawBorderH(item->x, item->y, item->width, item->height, border_width, border_colour, border_rounded);
			}
		}
		for (const auto& item : *items_capture)
		{
			item->draw();
		}
	}

	void Grid::getBounds(int16_t& x1, int16_t& y1, int16_t& x2, int16_t& y2) const
	{
		x1 = SHRT_MAX;
		y1 = SHRT_MAX;
		x2 = 0;
		y2 = 0;
		auto items_capture{ items };
		for (const auto& item : *items_capture)
		{
			if (item->x < x1)
			{
				x1 = item->x;
			}
			if (item->y < y1)
			{
				y1 = item->y;
			}
			auto item_x2 = item->x + item->width;
			auto item_y2 = item->y + item->height;
			if (item_x2 > x2)
			{
				x2 = item_x2;
			}
			if (item_y2 > y2)
			{
				y2 = item_y2;
			}
		}
	}

	void Grid::getDimensions(int16_t& x, int16_t& y, int16_t& width, int16_t& height) const
	{
		getBounds(x, y, width, height);
		width -= x;
		height -= y;
	}
}
