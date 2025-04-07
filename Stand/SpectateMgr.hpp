#pragma once

namespace Stand
{
	struct SpectateMgr
	{
		static void onTickSpectating(AbstractEntity ped);
		static void onStopSpectating();

		static void onTickSpectating();
		static void onTickWithAlteredFocus();
	};
}
