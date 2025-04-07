#pragma once

#include "AbstractPlayer.hpp"
#include "atFixedArray.hpp"
#include "CProjectile.hpp"
#include "CommandSliderFloat.hpp"
#include "pointers.hpp"

namespace Stand
{
	class ProjectileMgr
	{
	private:
		[[nodiscard]] static bool isRocketLauncher(Hash hash)
		{
			return hash == ATSTRINGHASH("WEAPON_RPG") || hash == ATSTRINGHASH("WEAPON_HOMINGLAUNCHER");
		}

		[[nodiscard]] static bool shouldProxyMine(CProjectile* projectile)
		{
			return proximity_mines->m_on
				&& projectile->m_bProximityMineTriggered
				&& projectile->m_uWeaponFiredFromHash == ATSTRINGHASH("WEAPON_PROXMINE")
				;
		}

		[[nodiscard]] static bool shouldProxyRocket(CProjectile* projectile)
		{
			if (proximity_rockets->m_on && isRocketLauncher(projectile->m_uWeaponFiredFromHash))
			{
				const auto projectile_pos = projectile->matrix.getTranslate();

				for (const auto& p : AbstractPlayer::listExcept(true))
				{
					if (auto ped = p.getPed(); ped.isValid())
					{
						if (!ped.isDead()
							&& ENTITY::GET_ENTITY_ALPHA(ped) == 255
							&& p.getPos().distance(projectile_pos) < proximity_rocket_fuse->getFloatValue()
							&& g_player_ped.getPos().distance(projectile_pos) > 10.0f
							&& ped.isInLineOfSight(projectile_pos) // Only proximity fuse when we have LoS.
							)
						{
							return true;
						}
					}
				}
			}

			return false;
		}

	public:
		static inline uint8_t handler_amount;

		static inline CommandToggle* proximity_mines;
		static inline CommandToggle* proximity_rockets;
		static inline CommandSliderFloat* proximity_rocket_fuse;

		static void onTick()
		{
			for (int i = 0; i != pointers::projectile_pool->m_Count; ++i)
			{
				if (auto proj = pointers::projectile_pool->elements[i])
				{
					if (proj->owner == (CEntity*)g_player_ped.getPointer()
						&& (shouldProxyMine(proj) || shouldProxyRocket(proj))
						)
					{
						pointers::CProjectile_TriggerExplosion(proj, 0);
					}
				}
			}
		}
	};
}