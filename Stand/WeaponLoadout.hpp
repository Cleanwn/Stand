#pragma once

#include <vector>

#include "fwddecl.hpp"
#include "natives_decl.hpp"

#include "WeaponLoadoutTree.hpp"

namespace Stand
{
	struct SavedWeapon
	{
		hash_t hash;
		int ammo;
		int tint;
		std::vector<hash_t> components;
	};

	class WeaponLoadout
	{
	private:
		std::vector<SavedWeapon> weapons;

	public:
		[[nodiscard]] static WeaponLoadout fromPed(AbstractEntity& ped);
		void apply(AbstractEntity& ped) const;
		void applySingleWeapon(AbstractEntity& p, hash_t weapon_hash) const;

		[[nodiscard]] soup::UniquePtr<WeaponLoadoutTree> toTree() const;
		[[nodiscard]] static WeaponLoadout fromTree(const soup::TreeReader& tr, const void* root);

		void clear() noexcept;
	};
}
