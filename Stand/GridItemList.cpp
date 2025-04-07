#include "GridItemList.hpp"

#include <algorithm>

#include "AntiDbg.hpp"
#include "ColourUtil.hpp"
#include "CommandInput.hpp"
#include "CommandColour.hpp"
#include "CommandList.hpp"
#include "CommandListSelect.hpp"
#include "CommandReadonlyValue.hpp"
#include "CommandSlider.hpp"
#include "CommandTextslider.hpp"
#include "CommandToggle.hpp"
#include "CommandToggleCustom.hpp"
#include "CommandToggleNoCorrelation.hpp"
#include "Exceptional.hpp"
#include "MenuGrid.hpp"
#include "Gui.hpp"
#include "input.hpp"
#include "Renderer.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	void DrawCommandData::setSlider(std::wstring&& value, std::wstring& name, LONG& command_name_max_width)
	{
		if (g_menu_grid.number_sliders_rightbound_values)
		{
			rightbound_text = std::move(value);
			rightbound_text_width = g_renderer.getTextWidth(rightbound_text, g_renderer.command_text.scale);
			command_name_max_width -= (LONG)rightbound_text_width;
		}
		else
		{
			name.append(L": ").append(std::move(value));
		}
	}

	GridItemList::GridItemList(CommandList* view, int16_t height, uint8_t priority, Alignment alignment_relative_to_last, cursor_t offset)
		: GridItem(GRIDITEM_LIST, g_renderer.command_width, height, priority, alignment_relative_to_last), view(view), draw_data(soup::make_shared<DrawListData>()), offset(offset)
	{
		update();
	}

	void GridItemList::update()
	{
		auto updated_draw_data = soup::make_shared<DrawListData>();
		cursor_t emul_cursor = updated_draw_data->offset = view->m_offset + this->offset;
		if (g_gui.lerp != 0 && emul_cursor != 0)
		{
			updated_draw_data->has_extra_top = true;
			--emul_cursor;
		}
		cursor_t draw_cursor = 0;
		for (std::vector<std::unique_ptr<Command>>::const_iterator i; i = view->children.cbegin() + emul_cursor, emul_cursor < (cursor_t)view->children.size(); ++emul_cursor)
		{
			const bool focused = (view->m_cursor == emul_cursor);
			auto* const _command = i->get();
			DrawCommandData draw_data{};
			draw_data.textColour = (focused ? g_renderer.getFocusTextColour() : g_renderer.getBgTextColour());
			draw_data.rightTextColour = (focused ? g_renderer.getFocusRightTextColour() : g_renderer.getBgRightTextColour());
			DirectX::SimpleMath::Color spriteColour = (focused ? g_renderer.getFocusSpriteColour() : g_renderer.getBgSpriteColour());
			if (focused)
			{
				draw_data.focused = true;
				if (g_renderer.preview_colour_in_list && view->type == COMMAND_LIST_COLOUR)
				{
					DirectX::SimpleMath::Color colour = ((CommandColourCustom*)view)->getRGBA();
					if (colour != g_renderer.getBgRectColour())
					{
						draw_data.hasRectColourOverride = true;
						draw_data.rectColourOverride = std::move(colour);
						ColourUtil::negateIfInsufficientContrast(draw_data.textColour, draw_data.rectColourOverride);
						ColourUtil::negateIfInsufficientContrast(draw_data.rightTextColour, draw_data.rectColourOverride);
						ColourUtil::negateIfInsufficientContrast(spriteColour, draw_data.rectColourOverride);
					}
				}
			}

			std::wstring name;
			std::wstring compact_name;
			LONG command_name_max_width = width - (5 + (g_menu_grid.left_space_before_all_commands ? g_renderer.command_height : 5));
			if (auto* const command = _command->getPhysical())
			{
				/*if (command == _command && command->isConcealed())
				{
					break;
				}*/
				name = command->menu_name.getLocalisedUtf16();
				if (Input::keyboard_and_mouse && !g_gui.hotkeys_disabled)
				{
					for (const auto& hotkey : command->hotkeys)
					{
						name.append(L" [").append(StringUtils::utf8_to_utf16(hotkey.toString())).push_back(L']');
					}
				}
				if (command->isToggle())
				{
					auto* const toggle = command->as<CommandToggleNoCorrelation>();
					command_name_max_width -= drawSpriteCommandRight(
						draw_data,
						(
							(
								(toggle->isT<CommandToggle>() && toggle->as<CommandToggle>()->correlation.isActive())
								|| (toggle->isT<CommandToggleCustom>() && toggle->as<CommandToggleCustom>()->auto_indicator)
							)
							? (toggle->m_on ? &g_renderer.textureToggleOnAuto : &g_renderer.textureToggleOffAuto)
							: (toggle->m_on ? &g_renderer.textureToggleOn : &g_renderer.textureToggleOff)
						),
						spriteColour,
						g_menu_grid.leftbound_textures_toggles
					);
				}
				else if (command->isList())
				{
					auto* const list = (const CommandList*)command;
					switch(list->type)
					{
					default:
						if (list->indicator_type != LISTINDICATOR_ARROW_IF_CHILDREN
							|| list->countVisibleChildren() != 0
							)
						{
							DirectX::SimpleMath::Color arrowSpriteColour = spriteColour;
							if (g_renderer.preview_colour_in_sprite && list->type == COMMAND_LIST_COLOUR)
							{
								const DirectX::SimpleMath::Color commandColour = ((const CommandColourCustom*)command)->getRGBA();
								if (!focused || ColourUtil::isContrastSufficient(commandColour, g_renderer.getFocusRectColour()))
								{
									arrowSpriteColour = commandColour;
								}
							}
							Texture* tex = &g_renderer.textureList;
							if (list->indicator_type == LISTINDICATOR_OFF)
							{
								tex = &g_renderer.textureToggleOffList;
							}
							else if (list->indicator_type == LISTINDICATOR_ON)
							{
								tex = &g_renderer.textureToggleOnList;
							}
							command_name_max_width -= drawSpriteCommandRight(
								draw_data,
								tex,
								arrowSpriteColour,
								g_menu_grid.leftbound_textures_nontoggles
							);
						}
						break;

					case COMMAND_LIST_SELECT:
						{
							auto current_value_label = ((const CommandListSelect*)command)->getCurrentValueMenuName();
							auto current_value_label_utf16 = current_value_label.getLocalisedUtf16();
							name.append(L": ");
							if (focused)
							{
								compact_name = name + current_value_label_utf16;
								name.append(L"< ");
							}
							name.append(current_value_label_utf16);
							if (focused)
							{
								name.append(L" >");
							}
							command_name_max_width -= drawSpriteCommandRight(
								draw_data,
								g_renderer.getSpriteByMenuName(current_value_label),
								spriteColour,
								g_menu_grid.leftbound_textures_nontoggles
							);
						}
						break;

					case COMMAND_LIST_SEARCH:
						{
							command_name_max_width -= drawSpriteCommandRight(
								draw_data,
								&g_renderer.textureSearch,
								spriteColour,
								g_menu_grid.leftbound_textures_nontoggles
							);
						}
						break;
					}
				}
				else if (command->isSlider())
				{
					std::wstring value{};
					{
						auto* const slider = (const CommandSlider*)command;
						if (slider->min_value == slider->max_value)
						{
							value = LANG_GET_W("NA");
						}
						else
						{
							value = slider->formatNumber(slider->value);
							if (focused)
							{
								value.insert(0, L"< ").append(L" >");
							}
						}
					}
					draw_data.setSlider(std::move(value), name, command_name_max_width);
				}
				else if (command->isT<CommandInput>())
				{
					auto value = StringUtils::utf8_to_utf16(command->as<CommandInput>()->value);
					if (!value.empty())
					{
						name.append(L": ").append(std::move(value));
					}
					command_name_max_width -= drawSpriteCommandRight(
						draw_data,
						&g_renderer.textureEdit,
						spriteColour,
						g_menu_grid.leftbound_textures_nontoggles
					);
				}
				else if (command->type == COMMAND_DIVIDER)
				{
					draw_data.centered = true;
				}
				else if (command->type == COMMAND_READONLY_VALUE)
				{
					if (!command->as<CommandReadonlyValue>()->value.empty())
					{
						const float value_width = g_renderer.getTextWidth(command->as<CommandReadonlyValue>()->value, g_renderer.command_text.scale);
						if (value_width > float(command_name_max_width) - (g_renderer.getTextWidth(name, g_renderer.command_text.scale) + 5))
						{
							name.append(L": ").append(command->as<CommandReadonlyValue>()->value);
						}
						else
						{
							draw_data.rightbound_text = command->as<CommandReadonlyValue>()->value;
							draw_data.rightbound_text_width = value_width;
							command_name_max_width -= (LONG)value_width;
						}
					}
				}
				else if (command->type == COMMAND_ACTION_ITEM)
				{
					command_name_max_width -= drawSpriteCommandRight(
						draw_data,
						g_renderer.getSpriteByMenuName(command->menu_name),
						spriteColour,
						g_menu_grid.leftbound_textures_nontoggles
					);
				}
				else if (command->type == COMMAND_READONLY_LINK)
				{
					command_name_max_width -= drawSpriteCommandRight(
						draw_data,
						&g_renderer.textureLink,
						spriteColour,
						g_menu_grid.leftbound_textures_nontoggles
					);
				}
				else if (command->type == COMMAND_TEXTSLIDER)
				{
					if (focused && command->as<CommandTextslider>()->hasOptions())
					{
						draw_data.rightbound_text = std::move(std::wstring(L"< ").append(command->as<CommandTextslider>()->getCurrentValueLabel().getLocalisedUtf16()).append(L" >"));
						draw_data.rightbound_text_width = g_renderer.getTextWidth(draw_data.rightbound_text, g_renderer.command_text.scale);
						command_name_max_width -= (LONG)draw_data.rightbound_text_width;
					}
				}
				else if (command->type == COMMAND_TEXTSLIDER_STATEFUL)
				{
					if (command->as<CommandTextslider>()->hasOptions())
					{
						auto value = command->as<CommandTextslider>()->getCurrentValueLabel().getLocalisedUtf16();
						if (focused
							&& command->as<CommandTextslider>()->options.size() != 1
							)
						{
							value.insert(0, L"< ").append(L" >");
						}
						draw_data.setSlider(std::move(value), name, command_name_max_width);
					}
				}
				StringUtils::replace_all(name, L"\n", L" ");
			}
			else
			{
				name = LANG_GET_W("NPHYS");
			}
		_retrim_name:
			draw_data.trimmed_name = name;
			if (g_renderer.trimTextH(draw_data.trimmed_name, g_renderer.command_text.scale, float(command_name_max_width)))
			{
				if (!compact_name.empty())
				{
					name = std::move(compact_name);
					compact_name.clear();
					goto _retrim_name;
				}
				if (focused)
				{
					if (_command->shouldShowUntrimmedName())
					{
						g_menu_grid.untrimmed_menu_name = std::move(name);
					}
					else
					{
						g_menu_grid.untrimmed_menu_name.clear();
					}
				}
			}
			else
			{
				if (focused)
				{
					g_menu_grid.untrimmed_menu_name.clear();
				}
			}
			updated_draw_data->list.emplace_back(std::move(draw_data));
			if (++draw_cursor == (g_gui.command_rows + (g_gui.lerp != 0) + updated_draw_data->has_extra_top))
			{
				break;
			}
		}

		// Ensure that (list.begin() + has_extra_top <= list.end()) holds true for when `draw` iterates the list
		SOUP_IF_UNLIKELY (updated_draw_data->list.empty())
		{
			updated_draw_data->has_extra_top = false;
		}

		if (AntiDbg::wasDebuggerCheckedRecently())
		{
			draw_data = std::move(updated_draw_data);
		}
	}

	LONG GridItemList::drawSpriteCommandRight(DrawCommandData& draw_data, Texture* texture, const DirectX::SimpleMath::Color& colour, bool leftbound)
	{
		if (texture == nullptr)
		{
			return 0;
		}
		if (leftbound)
		{
			draw_data.leftbound_texture = texture;
		}
		else
		{
			draw_data.rightbound_texture = texture;
		}
		draw_data.texture_colour = colour;
		auto width_subtrahend = LONG(texture->getRenderWidth((float)g_renderer.command_height));
		if (g_menu_grid.left_space_before_all_commands)
		{
			// transpose 'width - (5 + command_height)' to 'width - (5 + texture_width)'
			width_subtrahend -= g_renderer.command_height;
		}
		else
		{
			// transpose 'width - (5 + 5)' to 'width - (5 + texture_width)'
			width_subtrahend -= 5;
		}
		return width_subtrahend;
	}

	bool GridItemList::containsCommand(const Command* target) const noexcept
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(g_gui.root_mtx.isLockedByThisThread());
#endif
		cursor_t draw_cursor = 0;
		auto i = view->children.begin() + view->m_offset + this->offset;
		while (i != view->children.end())
		{
			if (target == i->get())
			{
				return true;
			}
			if (++draw_cursor == g_gui.command_rows)
			{
				break;
			}
			++i;
		}
		return false;
	}

	void GridItemList::draw()
	{
		drawBackgroundEffects();

		auto cursorPosH = g_renderer.getCursorPosH(false);
		if (g_gui.mouse_mode == MouseMode::NAVIGATE)
		{
			EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
			cursor_t emul_cursor = view->m_offset + this->offset;
			cursor_t draw_cursor = 0;
			for (std::vector<std::unique_ptr<Command>>::iterator i; i = view->children.begin() + emul_cursor, i < view->children.end(); ++emul_cursor)
			{
				LONG x1 = x;
				LONG y1 = y + (g_renderer.command_height * (LONG)draw_cursor);
				LONG x2 = x1 + width;
				LONG y2 = y1 + g_renderer.command_height;
				if (cursorPosH.x > x1 && x2 >= cursorPosH.x && cursorPosH.y > y1 && y2 >= cursorPosH.y)
				{
					const bool focused = (view->m_cursor == emul_cursor);
					if (focused)
					{
						if (g_renderer.mouse_squeaks)
						{
							g_renderer.mouse_squeaks = false;
							Exceptional::createManagedExceptionalThread(__FUNCTION__, []
							{
								g_gui.last_input_type = INPUTTYPE_MOUSE_CLICK;
								g_gui.inputClick(TC_OTHER);
							});
						}
					}
					else
					{
						if (auto focus_phys = view->getFocusPhysical(); focus_phys && !focus_phys->isSelectable() && g_gui.last_input_type == INPUTTYPE_INDIFFERENT)
						{
							// don't move cursor if we're about to be kicked off this command anyway
						}
						else
						{
							view->approachCursor(emul_cursor);
						}
					}
				}
				if (++draw_cursor == g_gui.command_rows)
				{
					break;
				}
			}
			EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
		}

		float focus_y = FLT_MIN;

		g_renderer.leaveDrawContext();
		auto posC = g_renderer.posH2C(x, y);
		auto sizeC = (g_renderer.posH2C(float(x + width), float(y + height)) - posC);
		contentsTex.ensureHasUnderlyingTextureOfSize((UINT)sizeC.x, (UINT)sizeC.y);
		if (contentsTex.hasUnderlyingTexture())
		{
			contentsTex.drawOnto([this, &focus_y]
			{
				g_renderer.enterDrawContext();

				auto draw_data_capture{ draw_data };
				int16_t draw_cursor = draw_data_capture->has_extra_top * -1;
				auto command_right = width;
				auto command_text_wrap_right = float(command_right - 5.0f);
				float menu_offset = 0.0f;
				if (g_gui.lerp_start != 0)
				{
					const auto lerp_perc = (float)std::min(GET_MILLIS_SINCE(g_gui.lerp_start), g_gui.lerp) / g_gui.lerp;
					menu_offset = std::lerp(float((draw_data_capture->offset - g_gui.lerp_from_offset - this->offset) * g_renderer.command_height), 0.0f, lerp_perc);
				}
				bool any_focused = false;
				for (auto draw_data = draw_data_capture->list.cbegin(); draw_data != draw_data_capture->list.cend(); ++draw_data)
				{
					if (draw_data->focused)
					{
						float cursor_offset = 0.0f;
						if (g_gui.lerp_start != 0)
						{
							const auto lerp_perc = (float)std::min(GET_MILLIS_SINCE(g_gui.lerp_start), g_gui.lerp) / g_gui.lerp;
							if (lerp_perc == 1.0f)
							{
								g_gui.lerp_start = 0;
							}
							else
							{
								cursor_offset = std::lerp(float((g_gui.lerp_from_cursor - draw_cursor - this->offset) * g_renderer.command_height), 0.0f, lerp_perc);
							}
						}
						g_renderer.drawRectC(DirectX::SimpleMath::Vector2{}, g_renderer.sizeH2C((float)width, float(g_renderer.command_height * draw_cursor) + cursor_offset), g_renderer.bgRectColour);
						g_renderer.drawRectC(g_renderer.sizeH2C(0.0f, float(g_renderer.command_height * (draw_cursor + 1)) + cursor_offset), g_renderer.sizeH2C((float)width, float(g_renderer.command_height * (draw_data_capture->list.size() - (g_gui.lerp != 0) - draw_data_capture->has_extra_top - draw_cursor)) - cursor_offset), g_renderer.bgRectColour);
						if (draw_cursor >= 0 && draw_cursor < g_gui.command_rows)
						{
							focus_y = float(g_renderer.command_height * draw_cursor) + cursor_offset;
							g_renderer.drawRectC(g_renderer.sizeH2C(0.0f, focus_y), g_renderer.sizeH2C((float)width, (float)g_renderer.command_height), (draw_data->hasRectColourOverride ? draw_data->rectColourOverride : g_renderer.focusRectColour));
						}
						any_focused = true;
						break;
					}
					++draw_cursor;
				}
				if (!any_focused)
				{
					g_renderer.drawRectC(DirectX::SimpleMath::Vector2{}, g_renderer.sizeH2C((float)width, (float)height), g_renderer.bgRectColour);
				}
				if (g_renderer.getCursorBorderWidth() != 0
					&& focus_y != FLT_MIN
					)
				{
					g_renderer.drawBorderC(g_renderer.sizeH2C(0.0f, focus_y), g_renderer.sizeH2C((float)width, (float)g_renderer.command_height), g_renderer.getCursorBorderWidth(), g_renderer.getCursorBorderColour(), g_renderer.getCursorBorderRounded());
				}
				draw_cursor = (draw_data_capture->has_extra_top * -1);
				for (auto draw_data = draw_data_capture->list.cbegin(); draw_data != draw_data_capture->list.cend(); ++draw_data)
				{
					auto draw_y = float(g_renderer.command_height * draw_cursor) + menu_offset;

					if (draw_data->leftbound_texture != nullptr)
					{
						auto texture_width = draw_data->leftbound_texture->getRenderWidth((float)g_renderer.command_height);
						g_renderer.drawSpriteC(*draw_data->leftbound_texture, g_renderer.sizeH2C(((float)g_renderer.command_height - texture_width), draw_y), g_renderer.sizeH2C(texture_width, g_renderer.command_height), draw_data->texture_colour);
					}

					if (draw_data->rightbound_texture != nullptr)
					{
						auto texture_width = draw_data->rightbound_texture->getRenderWidth((float)g_renderer.command_height);
						g_renderer.drawSpriteC(*draw_data->rightbound_texture, g_renderer.sizeH2C(command_right - texture_width, draw_y), g_renderer.sizeH2C(texture_width, g_renderer.command_height), draw_data->texture_colour);
					}
					else if (!draw_data->rightbound_text.empty())
					{
						g_renderer.drawTextHNoOffset(command_text_wrap_right - draw_data->rightbound_text_width, draw_y, draw_data->rightbound_text.c_str(), draw_data->rightTextColour, g_renderer.command_text);
					}

					if (draw_data->centered)
					{
						g_renderer.drawTextHNoOffset((float(width) * 0.5f), draw_y, std::wstring(draw_data->trimmed_name), draw_data->textColour, g_renderer.command_text, ALIGN_TOP_CENTRE, false);
					}
					else
					{
						g_renderer.drawTextHNoOffset((draw_data->leftbound_texture || g_menu_grid.left_space_before_all_commands) ? (float)g_renderer.command_height : 5.0f, draw_y, draw_data->trimmed_name.c_str(), draw_data->textColour, g_renderer.command_text);
					}

					++draw_cursor;
				}
				g_renderer.leaveDrawContext();
			});
		}
		g_renderer.enterDrawContext();
		if (g_renderer.getCursorBorderWidth() != 0
			&& focus_y != FLT_MIN
			)
		{
			g_renderer.drawBorderH((float)x, float(y) + focus_y, (float)width, (float)g_renderer.command_height, g_renderer.getCursorBorderWidth(), g_renderer.getCursorBorderColour(), g_renderer.getCursorBorderRounded());
		}
		g_renderer.drawSpriteC(contentsTex, (LONG)posC.x, (LONG)posC.y, contentsTex.width, contentsTex.height);
	}
}
