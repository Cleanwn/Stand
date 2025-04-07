#pragma once

#include <Windows.h>

#include "ScriptFuncPatch.hpp"

namespace Stand
{
	class DisableDripfeed
	{
	public:
		inline static BOOL enabled = true;

	private:
		inline static ScriptFuncPatch patch;

	public:
		static void onTick();
		static void onDisable();
	};
}
