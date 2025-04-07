#pragma once

#include "CommandPlayerActionAggressive.hpp"

#include "eDamageFlags.hpp"

namespace Stand
{
	class CommandPlayerKill : public CommandPlayerActionAggressive
	{
	public:
		explicit CommandPlayerKill(CommandList* const parent)
			: CommandPlayerActionAggressive(parent, LOC("PLYKLL"), CMDNAMES_OBF("kill"), LOC("PLYKLL_H"))
		{
		}

		static const uint64_t flags = eDamageFlags::MeleeDamage
			| eDamageFlags::ForceMeleeDamage
			| eDamageFlags::IgnorePedFlags
			| eDamageFlags::ForceInstantKill
			| eDamageFlags::IgnoreArmor
			| eDamageFlags::IgnoreStatModifiers
			| eDamageFlags::FatalMeleeDamage
			| eDamageFlags::SuppressImpactAudio
			| eDamageFlags::ExpectedPlayerKill
			| eDamageFlags::DontReportCrimes
			| eDamageFlags::IgnoreRemoteDistCheck;

		void onClick(Click& click) final
		{
			for (const auto& p : PP_PTR->getPlayers())
			{
				if (auto ped = p.getCPedPtr())
				{
					if (p == g_player)
					{
						ENTITY::SET_ENTITY_HEALTH(g_player_ped, 0, 0, 0);
					}
					else
					{
						for (uint8_t i = 0; i != 3; ++i)
						{
							const auto pos = p.getPos();
							pointers::CWeaponDamageEvent_Trigger(
								(CEntity*)g_player_ped.getCPed(), // pParentEntity
								ped, // pHitEntity
								&pos, // worldHitPosition
								0, // hitComponent
								true, // bOverride
								ATSTRINGHASH("weapon_unarmed"), // weaponHash
								9999.0f, // weaponDamage
								0, // tyreIndex
								0, // suspensionIndex
								flags, // damageFlags
								0, // actionResultId
								0, // meleeId
								0, // nForcedReactionDefinitionId
								false, // hitEntityWeapon
								false, // hitWeaponAmmoAttachment
								true, // silenced
								true, // firstBullet
								nullptr // hitDirection
							);
						}
					}
				}
			}
		}
	};
}