#pragma once

#include <cstdint>

namespace Stand
{
	enum TutorialState : uint8_t
	{
		TUT_OPEN,
		TUT_CLICK,
		TUT_BACK,
		TUT_DOWN,
		TUT_UP,
		TUT_ROOT,
		TUT_ROOT_DOWN,
		TUT_ROOT_UP,
		TUT_DONE,

		TUT_CMD,
		TUT_SLIDER,
		TUT_PROFILES,
		TUT_CTX,
	};

	struct Tutorial
	{
		inline static uint8_t state = TUT_DONE;

		static void start();
		static void setState(uint8_t state);
		static void fastForward();
		static void update();
	};
}
