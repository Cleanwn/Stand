#pragma once

#include <cstdint>

#include "fwddecl.hpp"
#include "typedecl.hpp"

#include "MenuKey.hpp"

namespace Stand
{
	class Pong
	{
	public:
		enum State : uint8_t
		{
			INACTIVE,
			INVITE_SENT,
			PLAYING,
			START_INTERMISSION,
			LOSE_INTERMISSION,
		};

		inline static constexpr int16_t platform_height = 280;
		inline static constexpr int16_t platform_width = 40;
		inline static constexpr int16_t platform_max_y = 1080 - platform_height;
		inline static constexpr int16_t platform_begin_y = platform_max_y / 2;
		inline static constexpr int16_t ball_size = 40;

		inline static int16_t left_platform_y = platform_begin_y;
		inline static int16_t right_platform_y = platform_begin_y;
		inline static int16_t ball_x = (1920 - ball_size) / 2;
		inline static int16_t ball_y = (1080 - ball_size) / 2;
		inline static int16_t ball_vec_x = -48;
		inline static int16_t platform_vec_y = 0;
		inline static int16_t ball_vec_y = 0;

		inline static State state = INACTIVE;
		inline static time_t intermission_end = 0;
		inline static bool processing_logic = false;
		inline static time_t last_tick = 0;
		inline static time_t ms_passed = 0;
		inline static CommandPlayer* participant = nullptr;
		inline static bool i_am_left = true;
		inline static int16_t my_remote_platform_y = platform_begin_y;
		inline static int16_t remote_ball_vec_x = -48;

		[[nodiscard]] static bool isActive();
		
		static int16_t* myPlatformY();
		static int16_t* participantPlatformY();

		static void resetBall();
		static void stopGame();
		static void onPartyLoses();
		static void intermission();

		[[nodiscard]] static bool amIResponsibleForTheBall();
		[[nodiscard]] static bool isPlatformTouchingBall(const int16_t platform_y);

		static void onPresent();
	private:
		static void onPresentInner();
	public:
		static void onTick();
	private:
		static void updateBallY();
		static void onILose();
		static void sendPos();
	};
}
