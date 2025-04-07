#pragma once

#include <memory>
#include <vector>

#include "fwddecl.hpp"
#include "typedecl.hpp"

namespace Stand
{
	using player_sort_algo_t = bool(*)(const CommandPlayer* const a, const CommandPlayer* const b);

	struct PlayerListSort
	{
		inline static std::vector<playerflag_t> hoisted_flags;
		inline static bool flags_have_changed_since_last_sort = false;

		static bool discovery(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool slot(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool rank(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool money(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool distance(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool kd(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool name(const CommandPlayer* const a, const CommandPlayer* const b);
		static bool host_queue_impl(AbstractPlayer a, AbstractPlayer b);
		static bool host_queue(const CommandPlayer* const a, const CommandPlayer* const b);

		inline static player_sort_algo_t algo = &name;
		inline static bool desc = false;

		bool operator() (const std::unique_ptr<Command>& _a, const std::unique_ptr<Command>& _b);
		[[nodiscard]] static bool isUpToDate();
		static void update();

		[[nodiscard]] static bool isDistanceBased() noexcept;
	};
}
