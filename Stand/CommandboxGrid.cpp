#include "CommandboxGrid.hpp"

#include "ColourUtil.hpp"
#include "Commandbox.hpp"
#include "CommandExtraInfo.hpp"
#include "Exceptional.hpp"
#include "GridItemColourBox.hpp"
#include "GridItemColourBoxBordered.hpp"
#include "GridItemPrimaryText.hpp"
#include "GridItemText.hpp"
#include "GridItemCommandboxInput.hpp"
#include "GridItemTextBigCentre.hpp"
#include "GridItemTextBigCentreBordered.hpp"
#include "Gui.hpp"
#include "huddecl.hpp"
#include "input.hpp"
#include "lang.hpp"
#include "StringUtils.hpp"
#include "UnicodePrivateUse.hpp"
#include "TextColour.hpp"

namespace Stand
{
	CommandboxGrid::CommandboxGrid()
		: Grid((HUD_WIDTH / 2) - (width / 2), 100, spacer_size)
	{
	}

	void CommandboxGrid::clearCache() noexcept
	{
		cache_input.clear();
		cache_hasData = false;
		cache_data.clear();
	}

	static GridItem* addExtra(std::vector<std::unique_ptr<GridItem>>& items_draft, uint8_t offset, std::wstring&& text, int16_t width, int16_t height, uint8_t priority, Alignment alignment_relative_to_last = ALIGN_TOP_RIGHT, GridItem* force_alignment_to = nullptr)
	{
		return ((g_commandbox.colour_selector_cursor == (CommandboxGrid::num_colours + offset))
			? items_draft.emplace_back(std::make_unique<GridItemTextBigCentreBordered>(std::move(text), width, height, priority, alignment_relative_to_last, force_alignment_to)).get()
			: items_draft.emplace_back(std::make_unique<GridItemTextBigCentre>(std::move(text), width, height, priority, alignment_relative_to_last, force_alignment_to)).get());
	}

	void CommandboxGrid::populate(std::vector<std::unique_ptr<GridItem>>& items_draft)
	{
		items_draft.emplace_back(std::make_unique<GridItemPrimaryText>(width, 24, LANG_GET_W("CMDPRMPT")));

		auto text = g_commandbox.text.substr(0, g_commandbox.cursor);
		text.push_back(L'|');
		text.append(g_commandbox.text.substr(g_commandbox.cursor));
		items_draft.emplace_back(std::make_unique<GridItemCommandboxInput>(std::move(text), width, 2));

		if (!g_commandbox.isColourSelectorActive())
		{
			std::wstring current_command{};
			auto res = StringUtils::explode_with_delimiting(g_commandbox.text, L';');
			if (!res.empty())
			{
				current_command = res.back();
			}
			if (g_commandbox.cursor >= (g_commandbox.text.length() - current_command.length()))
			{
				UnicodePrivateUse::toGta(current_command);
				std::wstring command_name{ current_command };
				std::wstring args{};
				if (Gui::parseCommand(command_name, args))
				{
					//items_draft.emplace_back(std::make_unique<GridItemText>(fmt::format(L"command_name = {}, args = {}", command_name, args), width, 0, 3));
					if (command_name != cache_input)
					{
						if (command_name != cacheupdate_input)
						{
							cache_mtx.lock();
							if (cacheupdate_running)
							{
								cacheupdate_next_input = std::move(command_name);
								cache_mtx.unlock();
							}
							else
							{
								cacheupdate_input = std::move(command_name);
								cacheupdate_running = true;
								cache_mtx.unlock();
								cacheupdate_thread.awaitCompletion(); // thread may still be winding down even tho 'cacheupdate_running' was false
								cacheupdate_thread.start([](soup::Capture&&)
								{
									__try
									{
										bool loop = false;
										do
										{
#if COMPACT_COMMAND_NAMES
											auto data = g_gui.findCommandsWhereCommandNameStartsWithAsWeakrefs(StringUtils::utf16_to_utf8(g_commandbox_grid.cacheupdate_input));
#else
											auto data = g_gui.findCommandsWhereCommandNameStartsWithAsWeakrefs(g_commandbox_grid.cacheupdate_input);
#endif
											//Sleep(1000);
											g_commandbox_grid.cache_mtx.lock();
											{
												g_commandbox_grid.cache_data = std::move(data);
												g_commandbox_grid.cache_input = std::move(g_commandbox_grid.cacheupdate_input);
												g_commandbox_grid.cache_hasData = true;

												loop = !g_commandbox_grid.cacheupdate_next_input.empty();
												g_commandbox_grid.cacheupdate_running = loop;
												if (loop)
												{
													g_commandbox_grid.cacheupdate_input = std::move(g_commandbox_grid.cacheupdate_next_input);
													g_commandbox_grid.cacheupdate_next_input.clear();
												}
											}
											g_commandbox_grid.cache_mtx.unlock();
											g_commandbox_grid.update();
										} while (loop);
									}
									__EXCEPTIONAL()
									{
									}
								});
							}
						}
						if (!cache_hasData)
						{
							return;
						}
						command_name = cache_input;
					}
					bool collapsed = false;
					g_commandbox_grid.cache_mtx.lock();
					auto commands{ cache_data };
					g_commandbox_grid.cache_mtx.unlock();
					while (true)
					{
						//items_draft.emplace_back(std::make_unique<GridItemText>(fmt::to_wstring(commands.size()), width, 0, 3));
						if (commands.empty())
						{
							items_draft.emplace_back(std::make_unique<GridItemText>(fmt::format(fmt::runtime(LANG_GET_W("CMDUNK")), command_name), width, 0, 3));
						}
						else if (commands.size() > 1)
						{
							if (command_name.length() > 2 || collapsed)
							{
								size_t shown = 0;
								for (const auto& command : commands)
								{
									EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
									if (auto cmd = command.getPointer())
									{
										items_draft.emplace_back(std::make_unique<GridItemText>(cmd->getCompletionHint(), width, 0, 3));
									}
									else
									{
										items_draft.emplace_back(std::make_unique<GridItemText>(LANG_GET_W("NPHYS"), width, 0, 3));
									}
									EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
									if (++shown == max_shown_matching_commands)
									{
										break;
									}
								}
								if (size_t more = (commands.size() - shown); more != 0)
								{
									if (commands.size() > 100)
									{
										items_draft.emplace_back(std::make_unique<GridItemText>(LANG_GET_W("MANYMORE"), width, 0, 3));
									}
									else
									{
										items_draft.emplace_back(std::make_unique<GridItemText>(fmt::format(fmt::runtime(LANG_GET_W("MORE")), more), width, 0, 3));
									}
								}
							}
						}
						else
						{
							CommandExtraInfo info{};
							EXCEPTIONAL_LOCK_READ(g_gui.root_mtx)
							if (auto cmd = commands.at(0).getPointer())
							{
								cmd->getExtraInfo(info, args);
							}
							else
							{
								info.completed_hint = LANG_GET_W("NPHYS");
							}
							EXCEPTIONAL_UNLOCK_READ(g_gui.root_mtx)
							if (info.collapse && !collapsed)
							{
								auto space_off = current_command.find(' ');
								if (space_off != std::string::npos)
								{
									command_name = current_command;
									command_name.erase(space_off, 1);
									space_off = command_name.find(' ');
									if (space_off != std::string::npos && command_name.length() > space_off + 1L)
									{
										args = command_name.substr(space_off + 1L);
									}
									else
									{
										args.clear();
									}
									StringUtils::to_lower(command_name);
#if COMPACT_COMMAND_NAMES
									commands = g_gui.findCommandsWhereCommandNameStartsWithAsWeakrefs(StringUtils::utf16_to_utf8(command_name));
#else
									commands = g_gui.findCommandsWhereCommandNameStartsWithAsWeakrefs(command_name);
#endif
									collapsed = true;
									continue;
								}
							}

							if (current_command.find(L' ') != std::string::npos)
							{
								const long long chars_left = (info.char_limit - args.length());
								if (info.char_limit != 0 && (!info.colour_selector || chars_left < 3))
								{
									items_draft.emplace_back(std::make_unique<GridItemText>(fmt::format(
										fmt::runtime(LANG_GET_W("CHRCNT")),
										fmt::arg(L"cur", args.length()),
										fmt::arg(L"max", info.char_limit)
									), width, 0, 4));
									break;
								}
								if (info.colour_selector)
								{
									items_draft.emplace_back(std::make_unique<GridItemText>(StringUtils::utf8_to_utf16(LANG_FMT(
										"FRMTSEL_AVAIL",
										Hotkey('U', true, false, false).toBracketedString()
									)), width, 0, 4));
									break;
								}
							}
							if (!info.completed_hint.empty())
							{
								items_draft.emplace_back(std::make_unique<GridItemText>(std::move(info.completed_hint), width, 0, 3));
								break;
							}
						}
						break;
					}
				}
				else
				{
					clearCache();
				}
			}
		}
		else
		{
			GridItem* first_colour;
			uint8_t draw_cursor = 0;
			for (const auto& colour : TextColour::all)
			{
				DirectX::SimpleMath::Color dxcol{ float(colour.r) / 255, float(colour.g) / 255, float(colour.b) / 255, 1.0f };
				DirectX::SimpleMath::Color border_colour{ 1.0f, 1.0f, 1.0f, 1.0f };
				GridItem* item;
				if (draw_cursor == g_commandbox.colour_selector_cursor)
				{
					ColourUtil::negateIfInsufficientContrast(border_colour, dxcol);
					item = items_draft.emplace_back(std::make_unique<GridItemColourBoxBordered>(4, draw_cursor == 0 ? ALIGN_BOTTOM_LEFT : ALIGN_TOP_RIGHT, std::move(dxcol), std::move(border_colour))).get();
				}
				else
				{
					item = items_draft.emplace_back(std::make_unique<GridItemColourBox>(4, draw_cursor == 0 ? ALIGN_BOTTOM_LEFT : ALIGN_TOP_RIGHT, std::move(dxcol))).get();
				}
				if (draw_cursor == 0)
				{
					first_colour = item;
				}
				++draw_cursor;
			}

#define ADD_EXTRA(...) addExtra(items_draft, __VA_ARGS__);

			GridItem* reset = ADD_EXTRA(0, UnicodePrivateUse::rs, COLOURS_WIDTH(1), colour_size, 4, ALIGN_BOTTOM_LEFT, first_colour);
			ADD_EXTRA(1, UnicodePrivateUse::wanted_star, COLOURS_WIDTH(1), colour_size, 4);
			ADD_EXTRA(2, UnicodePrivateUse::lock, COLOURS_WIDTH(1), colour_size, 4);
			ADD_EXTRA(3, UnicodePrivateUse::rs_verified, COLOURS_WIDTH(2), colour_size, 4);
			ADD_EXTRA(4, UnicodePrivateUse::rs_created, COLOURS_WIDTH(2), colour_size, 4);
			ADD_EXTRA(5, UnicodePrivateUse::blank_box, COLOURS_WIDTH(2), colour_size, 4);
			ADD_EXTRA(6, UnicodePrivateUse::reset, COLOURS_WIDTH(1), colour_size, 4);
			ADD_EXTRA(7, UnicodePrivateUse::newline, COLOURS_WIDTH(1), colour_size, 4);

			std::wstring bottom_text{};
			if (g_commandbox.colour_selector_cursor == (CommandboxGrid::num_colours + 6))
			{
				bottom_text = LANG_GET_W("FRMTSEL_H_R");
			}
			else if (g_commandbox.colour_selector_cursor == (CommandboxGrid::num_colours + 7))
			{
				bottom_text = LANG_GET_W("FRMTSEL_H_N");
			}
			else
			{
				bottom_text = StringUtils::utf8_to_utf16(LANG_FMT(
					"FRMTSEL_H",
					Input::vk_to_string(VK_RETURN)
				));
			}
			items_draft.emplace_back(std::make_unique<GridItemText>(std::move(bottom_text), width, 0, 5, ALIGN_BOTTOM_LEFT, reset));
		}
	}
}
