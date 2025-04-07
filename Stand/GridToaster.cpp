#include "GridToaster.hpp"

#include <algorithm>

#include "Exceptional.hpp"
#include "get_current_time_millis.hpp"
#include "NotifyGrid.hpp"
#include "StringUtils.hpp"

namespace Stand
{
#define FLASH_MS 500

	[[nodiscard]] static time_t estimate_reading_time(const std::wstring& str)
	{
		double num_words = 0.0;
		for (const wchar_t& c : str)
		{
			if (c == ' ')
			{
				num_words += 1.0;
			}
			else if (c > 0xff)
			{
				num_words += 0.5;
			}
		}
		const double seconds_per_word = 60.0 / static_cast<double>(g_grid_toaster.reading_speed_wpm);
		return static_cast<time_t>(num_words * seconds_per_word * 1000.0) + g_grid_toaster.reading_start_delay;
	}

	void GridToaster::toast(const std::wstring& message) const
	{
		time_t display_time = std::clamp<time_t>(estimate_reading_time(message), notifydur_min, notifydur_max);
		time_t flash_time = FLASH_MS;
		EXCEPTIONAL_LOCK(g_notify_grid.notifies_mtx)
		for (auto& notify : g_notify_grid.notifies)
		{
			if (notify.text == message)
			{
				if (notify.live)
				{
					flash_time += get_current_time_millis();
				}
				notify.display_time = std::move(display_time);
				notify.flash_time = std::move(flash_time);
				if (g_notify_grid.notifies.size() == 1)
				{
					notify.expiry_time = 0;
				}
				goto finish_toast;
			}
		}
		g_notify_grid.notifies.emplace_back(NotifyData{
			message,
			std::move(display_time),
			std::move(flash_time)
		});
	finish_toast:;
		EXCEPTIONAL_UNLOCK(g_notify_grid.notifies_mtx)
		g_notify_grid.update();
	}

	void GridToaster::setPersistentToast(const std::wstring& message)
	{
		if (g_notify_grid.persistent_notify.empty())
		{
			g_notify_grid.persistent_notify_flash_until = get_current_time_millis() + FLASH_MS;
		}
		g_notify_grid.persistent_notify = message;
		g_notify_grid.update();
	}

	void GridToaster::clearPersistentToast()
	{
		g_notify_grid.persistent_notify.clear();
		g_notify_grid.update();
	}

	void GridToaster::flashPersistentToast()
	{
		auto flash_until = get_current_time_millis() + FLASH_MS;
		g_notify_grid.persistent_notify_flash_until = flash_until;
		auto items_capture{ g_notify_grid.items };
		if (!items_capture->empty())
		{
			((GridItemNotify*)items_capture->at(0).get())->flash_until = flash_until;
		}
	}
}
