#pragma once

#include <string>
#include <vector>

#include "typedecl.hpp"

namespace Stand
{
	struct Weapon
	{
		enum Category : uint8_t
		{
			MELEE = 0,
			THROWABLE,
			PISTOL,
			SMG,
			RIFLE,
			SHOTGUN,
			SNIPER,
			HEAVY,

			_NUM_CATEGORIES
		};

		const hash_t hash;
		const char* const label_hash_name;
		const char* const american_name;
		const Category category;
		const hash_t group;
		const hash_t ammo_reward_hash;
		const hash_t disambiguator;
		const unsigned int bullets_in_batch;
		
		explicit constexpr Weapon(hash_t hash, const char* label_hash_name, const char* american_name, Category category, hash_t group, hash_t ammo_reward_hash = 0, hash_t disambiguator = 0, unsigned int bullets_in_batch = 0) noexcept
			: hash(hash), label_hash_name(label_hash_name), american_name(american_name), category(category), group(group), ammo_reward_hash(ammo_reward_hash), disambiguator(disambiguator), bullets_in_batch(bullets_in_batch)
		{
		}

		[[nodiscard]] std::string getDisplayName() const;
		[[nodiscard]] std::string getName() const;
		[[nodiscard]] std::string getCategoryName() const;

		static const Weapon weapon_unarmed;
		static const std::vector<Weapon> weapons;

		[[nodiscard]] static const Weapon* find(const hash_t hash);

		[[nodiscard]] static std::vector<hash_t> getAllHashes();
		[[nodiscard]] static std::vector<hash_t> getAllHashesIncludingWeirdos();
		[[nodiscard]] static bool isValidHash(hash_t hash);

		[[nodiscard]] static std::string getCategoryName(Category cat) noexcept;
		[[nodiscard]] static const char* getCategoryAmericanName(Category cat) noexcept;

		[[nodiscard]] static const char* categoryToString(Category cat) noexcept;
	};
}
