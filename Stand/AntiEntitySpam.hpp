#pragma once

#include "gta_fwddecl.hpp"
#include "hashtype.hpp"
#include <unordered_map>
#include <unordered_set>

namespace Stand
{
	struct EntityAccumulatorState
	{
		std::unordered_map<hash_t, uint16_t> occurrences{};
		bool deleted_an_entity = false;
	};

	class AntiEntitySpam
	{
	public:
		inline static bool enabled = true;
		inline static bool use_cam = false;
	private:
		inline static bool enabled_stare_at_sky = false;
		inline static std::unordered_map<hash_t, time_t> antagonists{};

	public:
		static void onDisable();
		static void onTick();
	private:
		static void addEntity(EntityAccumulatorState& state, AbstractEntity& ent, hash_t k);
		static uint16_t processNumbers(const std::unordered_map<hash_t, uint16_t>& occurrences, uint16_t(*get_treshold)(hash_t));
		static void ensureStareAtSkyIsDisabled();

		[[nodiscard]] static std::string kToString(hash_t k);
	};
}
