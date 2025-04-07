#pragma once

namespace Stand
{
	class ControlMgr
	{
	public:
		inline static bool gun_freedom = false;
		inline static bool move_freedom = false;
		inline static bool rc_freecam = false;
		inline static bool mobile_radio = false;
	private:
		inline static bool has_hook = false;

		static bool needsHook() noexcept;
	public:
		static void update() noexcept;
	};
}
