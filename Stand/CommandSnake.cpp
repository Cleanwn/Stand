#include "CommandSnake.hpp"

#include <soup/macros.hpp>
#include <soup/rand.hpp>

#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "huddecl.hpp"
#include "is_session.hpp"
#include "lang.hpp"
#include "natives.hpp"
#include "regular_event.hpp"
#include "Renderer.hpp"

namespace Stand
{
	CommandSnake::CommandSnake(CommandList* const parent)
		: CommandListSelect(parent, LOC("SNAKE"), { CMDNAME("snake") }, NOLABEL, {
			{0, LOC("DOFF"), CMDNAMES("off")},
			{1, LOC("SNAKE_1"), CMDNAMES("whilewaiting", "waiting", "transitions")},
			{2, LOC("ON"), CMDNAMES("on")},
		}, 0)
	{
		resetGame();
	}

	void CommandSnake::onChange(Click& click, long long prev_value)
	{
		if (value != 0)
		{
			if (snake_mode == 0)
			{
				rePresentEvent::registerHandler(&onPresent);
			}
		}
		else if (snake_mode != 0)
		{
			playing = false;
			rePresentEvent::unregisterHandler(&onPresent);
		}
		snake_mode = value;
	}

	void CommandSnake::onPresent()
	{
		if (snake_mode == 1 && !is_session_transition_active(true) && snake_length < 10)
		{
			playing = false;
			return;
		}

		if (g_gui.opened)
		{
			g_renderer.drawCentredTextThisFrame(LANG_GET_W("SNAKE_C"));
			playing = false;
			return;
		}

		constexpr LONG square_size = HUD_HEIGHT / 9;
		//constexpr uint8_t row = 16;

		// Game logic
		const time_t this_frame = get_current_time_millis();
		if (!playing)
		{
			playing = true;
			ms_passed = 0;
		}
		else
		{
			ms_passed += this_frame - last_frame;
		}
		last_frame = this_frame;
		while (ms_passed >= g_gui.input_speed)
		{
			ms_passed -= g_gui.input_speed;
			if (enlarge == 0)
			{
				emptySnakeStart();
			}
			else
			{
				enlarge--;
			}
			switch (g_gui.last_directional_input)
			{
			case UP:
				moveUp(snake_head);
				if (updateSnakeHead())
				{
					squares[snake_head] = SNAKE_FROM_DOWN;
				}
				break;

			case DOWN:
				moveDown(snake_head);
				if (updateSnakeHead())
				{
					squares[snake_head] = SNAKE_FROM_UP;
				}
				break;

			default:
			case RIGHT:
				moveRight(snake_head);
				if (updateSnakeHead())
				{
					squares[snake_head] = SNAKE_FROM_LEFT;
				}
				break;

			case LEFT:
				moveLeft(snake_head);
				if (updateSnakeHead())
				{
					squares[snake_head] = SNAKE_FROM_RIGHT;
				}
				break;
			}
			if (g_gui.last_directional_input_processed != g_gui.last_directional_input)
			{
				g_gui.last_directional_input_processed = g_gui.last_directional_input;
				switch (g_gui.last_directional_input)
				{
				case UP:
				case DOWN:
					g_gui.queueSoundJob([]
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
					});
					break;

				case LEFT:
				case RIGHT:
					g_gui.queueSoundJob([]
					{
						AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_LEFT_RIGHT", "HUD_FRONTEND_DEFAULT_SOUNDSET", true);
					});
					break;
				}
			}
		}

		// Render game
		g_renderer.drawHudspaceBorder();
		uint8_t i = 0;
		for (LONG y = 0; y < HUD_HEIGHT; y += square_size)
		{
			for (LONG x = 0; x < HUD_WIDTH; x += square_size)
			{
				/*if (y != 0.0)
				{
					g_renderer.drawRect(x - 4.0, y - 4.0, square_size + 8.0, 8.0, g_renderer.focusRectColour);
				}
				if (x != 0.0)
				{
					g_renderer.drawRect(x - 4.0, y - 4.0, 8.0, square_size + 8.0, g_renderer.focusRectColour);
				}*/
				switch (squares[i])
				{
				case SNAKE_FROM_UP:
				case SNAKE_FROM_DOWN:
				case SNAKE_FROM_RIGHT:
				case SNAKE_FROM_LEFT:
					g_renderer.drawRectH(float(x + 4), float(y + 4), float(square_size - 8), float(square_size - 8), g_renderer.minigameColour);
					break;

				case FRUIT:
					{
						auto pos = g_renderer.C2S(g_renderer.posH2C(
							float(x + 4) + (square_size / 2),
							float(y + 4) + (square_size / 2)
						));
						auto size = g_renderer.C2S(g_renderer.sizeH2C(square_size - 8, square_size - 8));
						Circle::inst_100.setRadius(size.x / 2.0f);
						g_renderer.drawCircleS(pos.x, pos.y, Circle::inst_100, g_renderer.minigameColour);
					}
					break;
				}
				//g_renderer.drawText(x, y, fmt::to_wstring(i), 2.0, g_renderer.focusTextAndSpriteColour);
				i++;
			}
		}
	}

	void CommandSnake::resetGame()
	{
		SquareState default_squares[16 * 9] = {
			SNAKE_FROM_LEFT,SNAKE_FROM_LEFT,SNAKE_FROM_LEFT,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
			EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
		};
		snake_head = 2;
		snake_length = 2;
		g_gui.last_directional_input = RIGHT;
		for (uint8_t i = 0; i < COUNT(default_squares); i++)
		{
			squares[i] = default_squares[i];
		}
		generateFruit();
	}

	void CommandSnake::generateFruit()
	{
		std::vector<uint8_t> empty_squares;
		for (uint8_t i = 0; i < COUNT(squares); i++)
		{
			if (squares[i] == EMPTY)
			{
				empty_squares.emplace_back(i);
			}
		}
		squares[empty_squares.at(soup::rand(0, empty_squares.size() - 1))] = FRUIT;
	}

	void CommandSnake::emptySnakeStart()
	{
		int16_t i = snake_head;
		uint8_t j = 0;
		do
		{
			switch (squares[i])
			{
			case SNAKE_FROM_UP:
				moveUp(i);
				break;

			case SNAKE_FROM_DOWN:
				moveDown(i);
				break;

			case SNAKE_FROM_RIGHT:
				moveRight(i);
				break;

			case SNAKE_FROM_LEFT:
				moveLeft(i);
				break;
			}
		} while (++j < snake_length);
		squares[i] = EMPTY;
	}

	bool CommandSnake::updateSnakeHead()
	{
		switch (squares[snake_head])
		{
		case FRUIT:
			snake_length++;
			enlarge++;
			generateFruit();
			g_gui.queueSoundJob([]
			{
				AUDIO::PLAY_SOUND_FRONTEND(-1, "Object_Collect_Remote", "GTAO_FM_Events_Soundset", true);
			});
			[[fallthrough]];
		case EMPTY:
			return true;
		}
		resetGame();
		return false;
	}

	void CommandSnake::moveUp(int16_t& i)
	{
		i -= 16;
		if (i < 0)
		{
			i += COUNT(squares);
		}
	}

	void CommandSnake::moveDown(int16_t& i)
	{
		i += 16;
		if (i >= COUNT(squares))
		{
			i -= COUNT(squares);
		}
	}

	void CommandSnake::moveRight(int16_t& i)
	{
		if (++i % 16 == 0)
		{
			moveUp(i);
		}
	}

	void CommandSnake::moveLeft(int16_t& i)
	{
		if (i-- % 16 == 0)
		{
			moveDown(i);
		}
	}
}
