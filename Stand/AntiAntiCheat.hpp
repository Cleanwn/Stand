#pragma once

namespace Stand
{
	struct AntiAntiCheat
	{
		static void onEnteredOnline();
		static void onTick();
		static void onPresent();
		static void onPostTick();

		static void disableScriptHostTracking();
	};
}
