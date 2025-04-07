#include "MenuGrid.hpp"

#include "Exceptional.hpp"
#include "GridItemAddressbar.hpp"
#include "GridItemHeaderAnimation.hpp"
#include "GridItemHeaderLoading.hpp"
#include "GridItemList.hpp"
#include "GridItemQrcode.hpp"
#include "GridItemScrollbar.hpp"
#include "GridItemTabsHorizontal.hpp"
#include "GridItemTabsVertical.hpp"
#include "GridItemText.hpp"
#include "Gui.hpp"
#include "lang.hpp"
#include "RootNameMgr.hpp"
#include "StringUtils.hpp"
#include "Worker.hpp"

#define spacer_x spacer_size
#define spacer_y spacer_size

namespace Stand
{
	MenuGrid::MenuGrid()
		: Grid(default_origin, 3)
	{
	}

	GridItemList* MenuGrid::getColumn(const Command* target) const noexcept
	{
		if (g_gui.command_columns != 1)
		{
			auto items_capture{ items };
			for (const auto& item : *items_capture)
			{
				if (item->type == GRIDITEM_LIST)
				{
					if (static_cast<GridItemList*>(item.get())->containsCommand(target))
					{
						return static_cast<GridItemList*>(item.get());
					}
				}
			}
		}
		return static_cast<GridItemList*>(getItemByType(GRIDITEM_LIST));
	}

	void MenuGrid::updateAddress()
	{
		predraw_flags |= GRID_UPDATE_MENU_ADDRESSBAR;
	}

	void MenuGrid::updateList(const Command* target)
	{
		if (!content_update_queued && !list_update_queued)
		{
			list_update_queued = true;
			Worker::queueJob([target]
			{
				EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
				g_menu_grid.list_update_queued = false;
				auto* list = g_menu_grid.getColumn(target);
				if (list != nullptr)
				{
					list->update();
					auto* scrollbar = g_menu_grid.getItemByType(GRIDITEM_SCROLLBAR);
					if (scrollbar != nullptr)
					{
						((GridItemScrollbar*)scrollbar)->update();
					}
				}
				EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
			});
		}
	}

	void MenuGrid::doUpdateAddress(const std::vector<std::unique_ptr<GridItem>>& items)
	{
		__try
		{
			auto* addressbar = getItemByType(items, GRIDITEM_PRIMARYTEXT);
			if (addressbar != nullptr)
			{
				addressbar->onPositioned();
			}
		}
		__EXCEPTIONAL()
		{
		}
	}

	void MenuGrid::populate(std::vector<std::unique_ptr<GridItem>>& items_draft)
	{
		EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
		root_cursor = g_gui.root_cursor;

		auto* list = g_gui.getCurrentUiList();
		cursor_t children_to_fit = list ? list->countVisibleChildren() : 0;
		const int16_t columns_for_this_list = std::clamp<int16_t>((int16_t)ceilf((float)children_to_fit / (float)g_gui.command_rows), 1, g_gui.command_columns);

		int16_t full_menu_width = g_renderer.command_width;
		if (addressbar_width_affected_by_columns)
		{
			full_menu_width *= columns_for_this_list;
			full_menu_width += (spacer_x * (columns_for_this_list - 1));
		}
		if (g_renderer.tabs_pos == LEFT || g_renderer.tabs_pos == RIGHT)
		{
			full_menu_width += spacer_x + g_renderer.tabs_width;
		}

		GridItem* addressbar = nullptr;

		if (g_gui.isUnloadPending())
		{
			addressbar = items_draft.emplace_back(std::make_unique<GridItemPrimaryText>(full_menu_width, g_renderer.addressbar_height, StringUtils::utf8_to_utf16(RootNameMgr::stock_root_name))).get();

			main_view = items_draft.emplace_back(std::make_unique<GridItemText>(LANG_GET_W("UNLD_T"), full_menu_width, info_padding, 2)).get();
		}
		else if (g_gui.root_state == GUI_NONE
			|| g_gui.isRootUpdatePendingOrInProgress()
			)
		{
			addressbar = items_draft.emplace_back(std::make_unique<GridItemPrimaryText>(full_menu_width, g_renderer.addressbar_height, StringUtils::utf8_to_utf16(RootNameMgr::stock_root_name))).get();

			main_view = items_draft.emplace_back(std::make_unique<GridItemText>(LANG_GET_W("GENWAIT"), full_menu_width, info_padding, 2)).get();
		}
		else if (g_gui.isInBadBoyTimeout())
		{
			addressbar = items_draft.emplace_back(std::make_unique<GridItemPrimaryText>(full_menu_width, g_renderer.addressbar_height, StringUtils::utf8_to_utf16(RootNameMgr::stock_root_name))).get();

			main_view = items_draft.emplace_back(std::make_unique<GridItemText>(LANG_GET_W("BADBOY"), full_menu_width, info_padding, 2)).get();
		}
		else if (g_gui.isAwaitingSetHotkeyInput())
		{
			if (g_renderer.show_addressbar)
			{
				addressbar = items_draft.emplace_back(std::make_unique<GridItemAddressbar>(full_menu_width)).get();
			}

			auto* const command = g_gui.getCurrentMenuFocus();
			main_view = items_draft.emplace_back(std::make_unique<GridItemText>(StringUtils::utf8_to_utf16(LANG_FMT("HOTKEY_U", command->getPhysical()->getActivationName().getLocalisedUtf8())), full_menu_width, info_padding, 2)).get();
		}
		else if (g_gui.isPromptActive())
		{
			if (g_renderer.show_addressbar)
			{
				addressbar = items_draft.emplace_back(std::make_unique<GridItemAddressbar>(full_menu_width)).get();
			}

			main_view = items_draft.emplace_back(std::make_unique<GridItemText>(std::wstring(g_gui.prompt_text), full_menu_width, info_padding, 2)).get();
		}
		else
		{
			if (g_renderer.show_addressbar)
			{
				addressbar = items_draft.emplace_back(std::make_unique<GridItemAddressbar>(full_menu_width)).get();
			}

			GridItem* tabs_left;
			if (g_renderer.tabs_pos == LEFT)
			{
				tabs_left = items_draft.emplace_back(std::make_unique<GridItemTabsVertical>(3, ALIGN_BOTTOM_LEFT)).get();
			}
			else if (g_renderer.tabs_pos == TOP)
			{
				tabs_left = items_draft.emplace_back(std::make_unique<GridItemTabsHorizontal>(3, ALIGN_BOTTOM_LEFT)).get();
			}

			auto alignment_relative_to_tabs = (g_renderer.tabs_pos == LEFT ? ALIGN_TOP_RIGHT : ALIGN_BOTTOM_LEFT);

			const auto warning_i = g_gui.warnings.find(g_gui.root_cursor);
			if (warning_i != g_gui.warnings.end())
			{
				main_view = items_draft.emplace_back(std::make_unique<GridItemText>(std::wstring(warning_i->second.text), g_renderer.command_width, info_padding, 3, alignment_relative_to_tabs)).get();
			}
			else
			{
				if (children_to_fit == 0)
				{
					main_view = items_draft.emplace_back(std::make_unique<GridItemText>(LANG_GET_W("ETYLST"), g_renderer.command_width, info_padding, 3, alignment_relative_to_tabs)).get();
				}
				else
				{
					const int16_t first_column_height = g_renderer.command_height * (int16_t)std::min(children_to_fit, g_gui.command_rows);
					main_view = items_draft.emplace_back(std::make_unique<GridItemList>(list, first_column_height, 3, alignment_relative_to_tabs, 0)).get();

					for (int16_t i = 1; i != columns_for_this_list; ++i)
					{
						children_to_fit -= g_gui.command_rows;
						const int16_t this_column_height = g_renderer.command_height * (int16_t)std::min(children_to_fit, g_gui.command_rows);
						items_draft.emplace_back(std::make_unique<GridItemList>(list, this_column_height, 3, ALIGN_TOP_RIGHT, g_gui.command_rows * i));
					}

					auto info_text_alignment = g_renderer.info_text_alignment;

					SOUP_IF_UNLIKELY (!qrcode.empty())
					{
						info_text_alignment = ALIGN_TOP_LEFT;
						items_draft.emplace_back(std::make_unique<GridItemQrcode>(g_renderer.command_width, 3, qrcode));
					}

					switch (scrollbar_mode)
					{
					case ScrollbarMode::ENABLED_WHEN_NEEDED:
						if (list->countVisibleChildren() <= size_t(g_gui.getCommandsOnScreenLimit()))
						{
							break;
						}
						[[fallthrough]];
					case ScrollbarMode::ENABLED:
						items_draft.emplace_back(std::make_unique<GridItemScrollbar>(list, scrollbar_width, first_column_height, 4));
						if (addressbar_width_affected_by_scrollbar && addressbar != nullptr)
						{
							addressbar->width += (spacer_x + scrollbar_width);
						}
						break;
					}

					std::vector<std::wstring> corner{};
					if (!untrimmed_menu_name.empty())
					{
						corner.emplace_back(untrimmed_menu_name);
					}
					if (auto* const command = list->getFocus())
					{
						command->populateCorner(corner);
					}
					auto i = corner.begin();
					if (i != corner.end())
					{
						auto* info_text_force_alignment_to = main_view;
						int16_t info_text_width = info_text_alignment == ALIGN_BOTTOM_LEFT ? g_renderer.command_width : info_width;
						if (info_text_alignment == ALIGN_BOTTOM_RIGHT) // "Below Tabs"
						{
							if (g_renderer.tabs_pos == LEFT)
							{
								info_text_force_alignment_to = tabs_left;
							}
							else
							{
								info_text_force_alignment_to = nullptr;
							}
							info_text_alignment = ALIGN_BOTTOM_LEFT;
						}
						else if (info_text_alignment == ALIGN_TOP_RIGHT)
						{
							info_text_force_alignment_to = nullptr;
						}
						items_draft.emplace_back(std::make_unique<GridItemText>(std::move(*i), info_text_width, info_padding, 5, info_text_alignment, info_text_force_alignment_to));
						while (++i != corner.end())
						{
							items_draft.emplace_back(std::make_unique<GridItemText>(std::move(*i), info_text_width, info_padding, 5, ALIGN_BOTTOM_LEFT));
						}
					}
				}
			}

			if (g_renderer.tabs_pos == RIGHT)
			{
				items_draft.emplace_back(std::make_unique<GridItemTabsVertical>(4, ALIGN_TOP_RIGHT));
			}
			else if (g_renderer.tabs_pos == BOTTOM)
			{
				items_draft.emplace_back(std::make_unique<GridItemTabsHorizontal>(4, ALIGN_BOTTOM_LEFT, main_view));
			}

			populateHeader(items_draft, full_menu_width, 255, items_draft.empty() ? nullptr : items_draft.at(0).get());
		}

		EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
	}

	void MenuGrid::populateHeader(std::vector<std::unique_ptr<GridItem>>& items_draft, int16_t full_menu_width, uint8_t priority, GridItem* force_alignment_to)
	{
		switch (g_renderer.header_state)
		{
		case HeaderState::SHOW:
			items_draft.emplace_back(std::make_unique<GridItemHeaderAnimation>(full_menu_width, priority, force_alignment_to));
			break;

		case HeaderState::DOWNLOADING:
		case HeaderState::LOADING:
			items_draft.emplace_back(std::make_unique<GridItemHeaderLoading>(full_menu_width, priority, force_alignment_to));
			break;
		}
	}
}
