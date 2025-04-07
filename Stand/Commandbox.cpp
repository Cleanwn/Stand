#include "Commandbox.hpp"

#include "Click.hpp"
#include "CommandboxGrid.hpp"
#include "FiberPool.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "natives.hpp"
#include "StringUtils.hpp"
#include "TextColour.hpp"
#include "UnicodePrivateUse.hpp"
#include "Util.hpp"

namespace Stand
{
	bool Commandbox::shouldBlockGameInputs() const noexcept
	{
		return active || !enter_released || !escape_released;
	}

	bool Commandbox::isColourSelectorActive() const noexcept
	{
		return colour_selector_active;
	}

	void Commandbox::resetSecondaryStates()
	{
		Textbox::resetSecondaryStates();
		colour_selector_active = false;
		g_commandbox_grid.clearCache();
	}

	void Commandbox::onUpdate()
	{
		colour_selector_active = false;
		g_commandbox_grid.update();
	}

	void Commandbox::onLeft()
	{
		if (isColourSelectorActive())
		{
			if (colour_selector_cursor == 0)
			{
				colour_selector_cursor = CommandboxGrid::num_colours - 1;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours)
			{
				colour_selector_cursor = CommandboxGrid::num_colours_and_extras - 1;
			}
			else
			{
				--colour_selector_cursor;
			}
			g_commandbox_grid.update();
		}
		else
		{
			Textbox::onLeft();
		}
	}

	void Commandbox::onRight()
	{
		if (isColourSelectorActive())
		{
			if (colour_selector_cursor == CommandboxGrid::num_colours - 1)
			{
				colour_selector_cursor = 0;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours_and_extras - 1)
			{
				colour_selector_cursor = CommandboxGrid::num_colours;
			}
			else
			{
				++colour_selector_cursor;
			}
			g_commandbox_grid.update();
		}
		else
		{
			Textbox::onRight();
		}
	}

	void Commandbox::onUp()
	{
		if (isColourSelectorActive())
		{
			toggleColourSelectorRow();
		}
		else
		{
			Textbox::onUp();
		}
	}

	void Commandbox::onDown()
	{
		if (isColourSelectorActive())
		{
			toggleColourSelectorRow();
		}
		else
		{
			Textbox::onDown();
		}
	}

	void Commandbox::onEnter()
	{
		if (isColourSelectorActive())
		{
			std::wstring appendix{};
			if (colour_selector_cursor < CommandboxGrid::num_colours)
			{
				appendix.append(1, L'~');
				appendix.append(1, TextColour::all[colour_selector_cursor].code);
				appendix.append(1, L'~');
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 0)
			{
				appendix = UnicodePrivateUse::rs;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 1)
			{
				appendix = UnicodePrivateUse::wanted_star;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 2)
			{
				appendix = UnicodePrivateUse::lock;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 3)
			{
				appendix = UnicodePrivateUse::rs_verified;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 4)
			{
				appendix = UnicodePrivateUse::rs_created;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 5)
			{
				appendix = UnicodePrivateUse::blank_box;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 6)
			{
				appendix = UnicodePrivateUse::reset;
			}
			else if (colour_selector_cursor == CommandboxGrid::num_colours + 7)
			{
				appendix = UnicodePrivateUse::newline;
			}
			appendText(appendix);
		}
		else
		{
			active = false;
			onBlur();
			enter_released = false;
			if (text.empty())
			{
				return;
			}

			if (opened_for_chaining
				&& !g_gui.user_understands_command_chaining
				&& !history.empty()
				)
			{
				auto& last_command = history.back();
				if (last_command != text)
				{
					auto help_text = StringUtils::utf16_to_utf8(fmt::format(fmt::runtime(LANG_GET_W("HLP_CMDCHAIN")), fmt::format(L"{};{}", last_command, text)));
					FiberPool::queueJob([help_text{ std::move(help_text) }]
					{
						Util::show_corner_help(std::move(help_text), true);
					});

					g_gui.user_understands_command_chaining = true;
					g_gui.saveTutorialFlags();
				}
			}

			last_command_issuance = get_current_time_millis();
			FiberPool::queueJob([this]
			{
				auto text = std::move(this->text);
				resetInput();
				UnicodePrivateUse::toGta(text);
				auto submitted_text{ text };
				Click click(CLICK_COMMAND, TC_SCRIPT_YIELDABLE);
				if (g_gui.triggerCommands(std::move(text), click))
				{
					history.emplace_back(std::move(submitted_text));
				}
			});
		}
	}

	void Commandbox::onEscape()
	{
		if (isColourSelectorActive())
		{
			colour_selector_active = false;
			g_commandbox_grid.update();
		}
		else
		{
			Textbox::onEscape();
			active = false;
			onBlur();
			g_gui.queueSoundJob([]
			{
				AUDIO::PLAY_SOUND_FRONTEND(-1, "CANCEL", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
			});
		}
		escape_released = false;
	}

	void Commandbox::onCtrlU()
	{
		colour_selector_active = !colour_selector_active;
		g_commandbox_grid.update();
	}

	uint8_t Commandbox::transposeCursorDown(uint8_t cursor)
	{
		if (cursor < 1)
		{
			return CommandboxGrid::num_colours + 0;
		}
		else if (cursor < 2)
		{
			return CommandboxGrid::num_colours + 1;
		}
		else if (cursor < 3)
		{
			return CommandboxGrid::num_colours + 2;
		}
		else if (cursor < 5)
		{
			return CommandboxGrid::num_colours + 3;
		}
		else if (cursor < 7)
		{
			return CommandboxGrid::num_colours + 4;
		}
		else if (cursor < 9)
		{
			return CommandboxGrid::num_colours + 5;
		}
		else if (cursor < 10)
		{
			return CommandboxGrid::num_colours + 6;
		}
		return CommandboxGrid::num_colours + 7;
	}

	void Commandbox::toggleColourSelectorRow()
	{
		if (colour_selector_cursor < CommandboxGrid::num_colours)
		{
			colour_selector_cursor_pre_transpose = colour_selector_cursor;
			colour_selector_cursor = transposeCursorDown(colour_selector_cursor);
		}
		else
		{
			if (transposeCursorDown(colour_selector_cursor_pre_transpose) == colour_selector_cursor)
			{
				colour_selector_cursor = colour_selector_cursor_pre_transpose;
			}
			else
			{
				for (uint8_t i = 0; i < CommandboxGrid::num_colours; ++i)
				{
					if (transposeCursorDown(i) == colour_selector_cursor)
					{
						colour_selector_cursor = i;
						break;
					}
				}
			}
		}
		g_commandbox_grid.update();
	}
}
