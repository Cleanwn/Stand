#include "Pong.hpp"

#include <fmt/xchar.h>

#include <soup/rand.hpp>

#include "CommandPlayer.hpp"
#include "DirectPacket.hpp"
#include "get_current_time_millis.hpp"
#include "Gui.hpp"
#include "Renderer.hpp"

namespace Stand
{
	bool Pong::isActive()
	{
		return state != INACTIVE;
	}

	int16_t* Pong::myPlatformY()
	{
		return i_am_left ? &left_platform_y : &right_platform_y;
	}

	int16_t* Pong::participantPlatformY()
	{
		return !i_am_left ? &left_platform_y : &right_platform_y;
	}

	void Pong::resetBall()
	{
		ball_x = (1920 - ball_size) / 2;
		ball_y = (1080 - ball_size) / 2;
		remote_ball_vec_x = ball_vec_x = -48;
		ball_vec_y = 0;
	}

	void Pong::stopGame()
	{
		intermission_end = 0;
		Pong::state = Pong::INACTIVE;
		participant = nullptr;
		left_platform_y = platform_begin_y;
		right_platform_y = platform_begin_y;
		my_remote_platform_y = platform_begin_y;
		platform_vec_y = 0;
		resetBall();
	}

	void Pong::onPartyLoses()
	{
		state = Pong::LOSE_INTERMISSION;
		intermission();
		resetBall();
	}

	void Pong::intermission()
	{
		intermission_end = get_current_time_millis() + 3000;
	}

	bool Pong::amIResponsibleForTheBall()
	{
		return i_am_left ? remote_ball_vec_x <= 0 : remote_ball_vec_x > 0;
	}

	bool Pong::isPlatformTouchingBall(const int16_t platform_y)
	{
		return platform_y < ball_y + ball_size && (platform_y + platform_height) > ball_y;
	}

	void Pong::onPresent()
	{
		if (state != INACTIVE)
		{
			onPresentInner();
		}
	}

	void Pong::onPresentInner()
	{
		if (g_gui.opened)
		{
			g_renderer.drawCentredTextThisFrame(LANG_GET_W("PONG_C"));
		}
		else
		{
			switch (state)
			{
			case INVITE_SENT:
				g_renderer.drawCentredTextThisFrame(StringUtils::utf8_to_utf16(LANG_FMT("INVSENT", participant->getPlayerName())));
				break;

			case START_INTERMISSION:
				g_renderer.drawCentredTextThisFrame(LANG_GET_W("GETREADY"));
				break;

			case LOSE_INTERMISSION:
				g_renderer.drawCentredTextThisFrame(fmt::format(
					L"{} : {}",
					i_am_left ? participant->my_pong_wins : participant->their_pong_wins,
					!i_am_left ? participant->my_pong_wins : participant->their_pong_wins
				));
				break;
			}
		}

		if (state == PLAYING)
		{
			g_renderer.drawHudspaceBorder();
			g_renderer.drawRectH(float(ball_x), float(ball_y), float(ball_size), float(ball_size), g_renderer.minigameColour);
		}

		g_renderer.drawRectH(0.0f, float(left_platform_y), float(platform_width), float(platform_height), i_am_left ? g_renderer.minigameColour : participant->minigameColour);
		if (state != INVITE_SENT || !i_am_left)
		{
			g_renderer.drawRectH(float(1920 - platform_width), float(right_platform_y), float(platform_width), float(platform_height), !i_am_left ? g_renderer.minigameColour : participant->minigameColour);
		}
	}

	void Pong::onTick()
	{
		if (state == INACTIVE)
		{
			processing_logic = false;
			return;
		}
		if (participant->queued_for_deletion)
		{
			stopGame();
			processing_logic = false;
			return;
		}
		const long long this_tick = get_current_time_millis();
		if (!processing_logic)
		{
			processing_logic = true;
			ms_passed = 0;
		}
		else
		{
			ms_passed += this_tick - last_tick;
		}
		last_tick = this_tick;
		while (ms_passed >= 40)
		{
			ms_passed -= 40;

			if (!g_gui.opened)
			{
				switch (Gui::menuKeyToDirection2d(g_gui.keyboard_pressing))
				{
				default:
					if (ball_vec_x < 0)
					{
						platform_vec_y = 0;
					}
					break;

				case UP:
					if ((*myPlatformY() -= 50) < 0)
					{
						*myPlatformY() = 0;
					}
					if (ball_vec_x < 0)
					{
						platform_vec_y = -24;
					}
					break;

				case DOWN:
					if ((*myPlatformY() += 50) > platform_max_y)
					{
						*myPlatformY() = platform_max_y;
					}
					if (ball_vec_x < 0)
					{
						platform_vec_y = +24;
					}
					break;
				}
			}

			if (state == PLAYING && amIResponsibleForTheBall())
			{
				ball_y += ball_vec_y;
				if (ball_y < 0 && ball_vec_y < 0)
				{
					ball_y *= -1;
					ball_vec_y *= -1;
				}
				if (ball_y > 1080 - ball_size && ball_vec_y > 0)
				{
					ball_y -= ball_vec_y;
					ball_vec_y *= -1;
					ball_y += ball_vec_y;
				}
				ball_x += ball_vec_x;
				if (ball_x < platform_width)
				{
					if (isPlatformTouchingBall(left_platform_y))
					{
						if (ball_vec_x < 0)
						{
							ball_vec_x *= -1;
							ball_vec_x += 1;
							updateBallY();
						}
					}
					else if (ball_x < -ball_size)
					{
						onILose();
						return;
					}
				}
				else if (ball_x > 1920 - ball_size - platform_width)
				{
					if (isPlatformTouchingBall(right_platform_y))
					{
						if (ball_vec_x > 0)
						{
							ball_vec_x += 1;
							ball_vec_x *= -1;
							updateBallY();
						}
					}
					else if (ball_x > 1920)
					{
						onILose();
						return;
					}
				}
			}
		}
		if (state == PLAYING)
		{
			if (amIResponsibleForTheBall())
			{
				sendPos();
				remote_ball_vec_x = ball_vec_x;
			}
			else if (my_remote_platform_y != *myPlatformY())
			{
				sendPos();
			}
		}
		else if (intermission_end != 0 && get_current_time_millis() > intermission_end)
		{
			state = PLAYING;
			intermission_end = 0;
		}
	}

	void Pong::updateBallY()
	{
		if (platform_vec_y == 0)
		{
			ball_vec_y = soup::rand.t<int16_t>(0, 48);
		}
		else if (platform_vec_y > 0)
		{
			ball_vec_y = platform_vec_y + soup::rand.t<int16_t>(0, 24);
		}
		else // platform_vec_y < 0
		{
			ball_vec_y = platform_vec_y - soup::rand.t<int16_t>(0, 24);
		}
	}

	void Pong::onILose()
	{
		participant->getPlayer().directPacketSend(DP_PONG_I_LOSE, ++participant->their_pong_wins);
		onPartyLoses();
		ms_passed = 0;
	}

	void Pong::sendPos()
	{
		my_remote_platform_y = *myPlatformY();
		participant->getPlayer().directPacketSend(DP_PONG_POS, my_remote_platform_y, ball_x, ball_y, ball_vec_x, ball_vec_y);
	}
}
