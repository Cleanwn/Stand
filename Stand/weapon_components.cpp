#include "weapon_components.hpp"

#include "Util.hpp"

namespace Stand
{
	Label WeaponComponent::getName() const
	{
		if (nameIsLabelKey())
		{
			return LIT(Util::GET_LABEL_TEXT_no_replacements(rage::atStringHash(name)));
		}
		return LIT(name);
	}

	const char* WeaponComponent::getAmericanName() const
	{
		if (american_name)
		{
			return american_name;
		}
		return name;
	}

	bool WeaponComponent::isValid(hash_t hash) noexcept
	{
		switch (hash)
		{
		case ATSTRINGHASH("COMPONENT_GUNRUN_MK2_UPGRADE"):
		case ATSTRINGHASH("COMPONENT_FLASHLIGHT_LIGHT"):
			return true;
		}
		for (const auto& wc : g_weapon_components)
		{
			if (wc.hash.hash == hash)
			{
				return true;
			}
		}
		for (const auto& wl : g_weapon_liveries)
		{
			for (const auto& wc : wl.second)
			{
				if (wc.hash.hash == hash)
				{
					return true;
				}
			}
		}
		return false;
	}
}
