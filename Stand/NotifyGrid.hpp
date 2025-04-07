#pragma once

#include "Grid.hpp"

#include "get_current_time_millis.hpp"
#include "GridItemNotify.hpp"
#include "Spinlock.hpp"

namespace Stand
{
#pragma pack(push, 1)
	struct NotifyData
	{
		std::wstring text;
		time_t display_time;
		time_t flash_time;
		time_t expiry_time;
		bool live = false;
		uint8_t expiry_divisor = 1;
	};

	class NotifyGrid : public Grid
	{
	public:
		std::wstring persistent_notify;
		time_t persistent_notify_flash_until;
		Spinlock notifies_mtx;
		std::vector<NotifyData> notifies;

		bool invert_flow = false;
		int16_t notify_width = 400;
		int16_t notify_padding = 10;

		bool next_to_map = true;

		bool minimap_visible = false;
		bool bigmap = false;
		bool new_invites_indicator = false;
		float safe_zone = 0.3f;
		float client_size_y = 0.0f;

		explicit NotifyGrid();

		void onTick();
		void setOriginNextToMap();

		void populate(std::vector<std::unique_ptr<GridItem>>& items_draft) final;

		void clear();

		time_t frozen_since = 0;
		[[nodiscard]] bool isFrozen() const noexcept;
		[[nodiscard]] bool wasDeadlineReached(time_t deadline) const noexcept;
		void freeze();
		void unfreeze();
	};
#pragma pack(pop)

	inline NotifyGrid g_notify_grid = NotifyGrid();
}
