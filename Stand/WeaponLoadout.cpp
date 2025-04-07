#include "WeaponLoadout.hpp"

#include <unordered_set>

#include "AbstractEntity.hpp"
#include "joaatToString.hpp"
#include "natives.hpp"
#include "weapons.hpp"
#include "weapon_components.hpp"

namespace Stand
{
	WeaponLoadout WeaponLoadout::fromPed(AbstractEntity& ped)
	{
		WeaponLoadout loadout;
		for (const auto& weapon : Weapon::getAllHashesIncludingWeirdos())
		{
			if (WEAPON::HAS_PED_GOT_WEAPON(ped, weapon, false))
			{
				SavedWeapon w;
				w.hash = weapon;
				w.ammo = WEAPON::GET_AMMO_IN_PED_WEAPON(ped, weapon);
				w.tint = WEAPON::GET_PED_WEAPON_TINT_INDEX(ped, weapon);
				for (const auto& comp : g_weapon_components)
				{
					if (WEAPON::HAS_PED_GOT_WEAPON_COMPONENT(ped, weapon, comp.hash))
					{
						w.components.emplace_back(comp.hash);
					}
				}
				loadout.weapons.emplace_back(std::move(w));
			}
		}
		return loadout;
	}

	void WeaponLoadout::apply(AbstractEntity& ped) const
	{
		std::unordered_set<hash_t> owned;

		for (const auto& w : weapons)
		{
			owned.emplace(w.hash);
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, w.hash, w.ammo, false);
			WEAPON::SET_PED_WEAPON_TINT_INDEX(ped, w.hash, w.tint);
			for (const auto& comp : w.components)
			{
				WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(ped, w.hash, comp);
			}
		}

		for (const auto& weapon : Weapon::getAllHashesIncludingWeirdos())
		{
			if (!owned.contains(weapon))
			{
				WEAPON::REMOVE_WEAPON_FROM_PED(ped, weapon);
			}
		}
	}

	void WeaponLoadout::applySingleWeapon(AbstractEntity& p, hash_t weapon_hash) const
	{
		for (const auto& w : weapons)
		{
			if (w.hash == weapon_hash)
			{
				if (!WEAPON::HAS_PED_GOT_WEAPON(p, w.hash, FALSE))
				{
					WEAPON::GIVE_DELAYED_WEAPON_TO_PED(p, w.hash, w.ammo, false);
				}

				WEAPON::SET_PED_WEAPON_TINT_INDEX(p, w.hash, w.tint);

				for (const auto& comp : w.components)
				{
					WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(p, w.hash, comp);
				}
			}
		}
	}

	soup::UniquePtr<WeaponLoadoutTree> WeaponLoadout::toTree() const
	{
		auto root = soup::make_unique<WeaponLoadoutTree>();
		for (const auto& w : weapons)
		{
			auto wnode = soup::make_unique<WeaponLoadoutTree::Weapon>();
			wnode->hash = w.hash;
			wnode->tint = w.tint;
			for (const auto& comp : w.components)
			{
				auto cnode = soup::make_unique<WeaponLoadoutTree::Component>();
				cnode->hash = comp;
				wnode->children.emplace_back(std::move(cnode));
			}
			root->children.emplace_back(std::move(wnode));
		}
		return root;
	}

	WeaponLoadout WeaponLoadout::fromTree(const soup::TreeReader& tr, const void* root)
	{
		WeaponLoadout loadout;

		const size_t num_weapons = tr.getNumChildren(root);
		loadout.weapons.reserve(num_weapons);
		for (size_t i = 0; i != num_weapons; ++i)
		{
			const void* const wnode = tr.getChild(root, i);

			SavedWeapon w;
			w.hash = stringToJoaat(tr.getName(wnode));
			w.ammo = 9999;

			auto value = tr.getValue(wnode);
			if (value.substr(0, 5) == "Tint ")
			{
				w.tint = std::stoi(value.c_str() + 5);
			}
			else
			{
				w.tint = 0;
			}

			const size_t num_components = tr.getNumChildren(wnode);
			w.components.reserve(num_components);
			for (size_t j = 0; j != num_components; ++j)
			{
				w.components.emplace_back(stringToJoaat(tr.getName(tr.getChild(wnode, j))));
			}
			
			loadout.weapons.emplace_back(std::move(w));
		}

		return loadout;
	}

	void WeaponLoadout::clear() noexcept
	{
		weapons.clear();
	}
}
