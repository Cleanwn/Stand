#pragma once

#include "Toaster.hpp"

namespace Stand
{
	class GridToaster : public Toaster
	{
	public:
		uint16_t reading_speed_wpm = 250;
		time_t reading_start_delay = 500;
		time_t notifydur_min = 2000;
		time_t notifydur_max = 10000;

		void toast(const std::wstring& message) const final;

		void setPersistentToast(const std::wstring& message) final;
		void clearPersistentToast() final;

		static void flashPersistentToast();
	};

	inline GridToaster g_grid_toaster{};
}
