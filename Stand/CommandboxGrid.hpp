#pragma once

#include "Grid.hpp"

#include "fwddecl.hpp"

#include <soup/Thread.hpp>
#include <soup/WeakRef.hpp>

#include "Spinlock.hpp"

#define COLOURS_WIDTH(i) (((colour_size + spacer_size) * i) - spacer_size)

namespace Stand
{
	class CommandboxGrid : public Grid
	{
	public:
		static constexpr int16_t colour_size = 47;
		static constexpr int16_t spacer_size = 3;
		static constexpr int16_t border_width = 2;

		static constexpr int16_t num_colours = 11;
		static constexpr int16_t num_extras = 8;
		static constexpr int16_t num_colours_and_extras = num_colours + num_extras;

		static constexpr int16_t width = COLOURS_WIDTH(num_colours);

		Spinlock cache_mtx{};
		std::wstring cache_input{};
		bool cache_hasData = false;
		std::vector<soup::WeakRef<CommandIssuable>> cache_data{};
		bool cacheupdate_running = false;
		std::wstring cacheupdate_input{};
		std::wstring cacheupdate_next_input{};
		soup::Thread cacheupdate_thread{};

		size_t max_shown_matching_commands = 7;

		explicit CommandboxGrid();

		void clearCache() noexcept;

		void populate(std::vector<std::unique_ptr<GridItem>>& items_draft) final;
	};

	inline CommandboxGrid g_commandbox_grid = CommandboxGrid();
}
