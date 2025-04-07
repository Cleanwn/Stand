#pragma once

#include "CommandListSelect.hpp"

namespace Stand
{
	class CommandSnake : public CommandListSelect
	{
	private:
		enum SquareState : uint8_t
		{
			EMPTY,
			SNAKE_FROM_UP,
			SNAKE_FROM_DOWN,
			SNAKE_FROM_RIGHT,
			SNAKE_FROM_LEFT,
			FRUIT,
		};

	public:
		inline static bool playing = false;
	private:
		inline static long long snake_mode = 0;
		inline static time_t last_frame = 0;
		inline static time_t ms_passed = 0;
		inline static int16_t snake_head = 2;
		inline static SquareState squares[16 * 9] = { EMPTY };
		inline static uint8_t snake_length = 2;
		inline static uint8_t enlarge = 0;

	public:
		explicit CommandSnake(CommandList* const parent);

		void onChange(Click& click, long long prev_value) final;

	private:
		static void onPresent();

		static void resetGame();
		static void generateFruit();

		static void emptySnakeStart();
		[[nodiscard]] static bool updateSnakeHead();

		static void moveUp(int16_t& i);
		static void moveDown(int16_t& i);
		static void moveRight(int16_t& i);
		static void moveLeft(int16_t& i);
	};
}
