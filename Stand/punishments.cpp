#include "punishments.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "dedsec_hacks.hpp"
#include "drawDebugText.hpp"
#include "DrawUtil3d.hpp"
#include "eCommandApplyForceTypes.hpp"
#include "eExplosionTag.hpp"
#include "Exceptional.hpp"
#include "Firemangr.hpp"
#include "is_session.hpp"
#include "joaatToString.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "eTaskType.hpp"
#include "TickMgr.hpp"
#include "Util.hpp"

namespace Stand
{
	Punishment::Punishment(punishment_t mask, Label&& name, uint8_t _for, Label&& help_text, std::vector<CommandName>&& gunshoot_command_names)
		: mask(mask), name(std::move(name)), _for(_for), help_text(std::move(help_text)), gunshoot_command_names(std::move(gunshoot_command_names))
	{
	}

	bool Punishment::isApplicable(uint8_t _to) const noexcept
	{
		constexpr uint8_t target_selector = (PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_WORLD | PUNISHMENTFOR_NPCHOSTILITY);
		if (!((_for & target_selector) & _to))
		{
			return false;
		}
		if ((_for & PUNISHMENTFOR_AIMINGAUSER) && !(_to & PUNISHMENTFOR_AIMINGAUSER))
		{
			return false;
		}
		if ((_for & PUNISHMENTFOR_NOTBROAD) && !(_to & PUNISHMENTFOR_NOTBROAD))
		{
			return false;
		}
		if ((_for & PUNISHMENTFOR_USERACTION) && !(_to & PUNISHMENTFOR_USERACTION))
		{
			return false;
		}
		return true;
	}

	WorldPunishment::WorldPunishment(punishment_t bitflag, Label&& name, uint8_t _for, Label&& help_text)
		: Punishment(bitflag, std::move(name), _for | PUNISHMENTFOR_WORLD, std::move(help_text))
	{
	}

	void WorldPunishment::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		execute(ent.getPos(), ent, trigger);
	}

	PunishmentExplodeAnon::PunishmentExplodeAnon()
		: WorldPunishment(PUNISHMENT_EXPLODE_ANON, LOC("PNSH_EXPANON"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentExplodeAnon::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			execute(ent.getPos(), ent, trigger);
		}
	}

	void PunishmentExplodeAnon::execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!TickMgr::explosion_this_tick)
		{
			FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, EXP_TAG_STICKYBOMB, 1.0f, true, false, 0.0f, false);
			TickMgr::explosion_this_tick = is_session_started();
		}
	}

	PunishmentExplodeOwned::PunishmentExplodeOwned()
		: WorldPunishment(PUNISHMENT_EXPLODE_OWNED, LOC("PNSH_EXP"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentExplodeOwned::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			execute(ent.getPos(), ent, trigger);
		}
	}

	void PunishmentExplodeOwned::execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!TickMgr::explosion_this_tick)
		{
			FIRE::ADD_OWNED_EXPLOSION(g_player_ped, pos.x, pos.y, pos.z, EXP_TAG_STICKYBOMB, 1.0f, true, false, 0.0f);
			TickMgr::explosion_this_tick = is_session_started();
		}
	}

	PunishmentBurn::PunishmentBurn()
		: WorldPunishment(PUNISHMENT_BURN, LOC("PNSH_BRN"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentBurn::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		DedsecHack::burn.execute(ent);
	}

	void PunishmentBurn::execute(const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (Firemangr::script_fires.size() == 50)
		{
			FIRE::REMOVE_SCRIPT_FIRE(Firemangr::script_fires.front());
			Firemangr::script_fires.pop_front();
		}
		Firemangr::script_fires.emplace_back(FIRE::START_SCRIPT_FIRE(pos.x, pos.y, pos.z, 25, false));
	}

	PunishmentDie::PunishmentDie()
		: Punishment(PUNISHMENT_DIE, LOC("PNSH_DIE"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentDie::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			if (!ent.isAPlayer())
			{
				if (!ent.isOwner())
				{
					auto pos = ent.getPos();

					pointers::CWeaponDamageEvent_Trigger(
						(CEntity*)g_player_ped.getCPed(), // pParentEntity
						ent, // pHitEntity
						&pos, // worldHitPosition
						0, // hitComponent
						true, // bOverride
						ATSTRINGHASH("WEAPON_HEAVYSNIPER"), // weaponHash
						9999.0f, // weaponDamage
						0, // tyreIndex
						0, // suspensionIndex
						0, // damageFlags
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

				ent.requestControl(ATSTRINGHASH("die"), [](AbstractEntity& ped)
				{
					ped.setHealth(0.0f);
				});
			}
			else
			{
				DedsecHack::kill.execute(ent);
			}
		}
	}

	PunishmentUnarm::PunishmentUnarm()
		: Punishment(PUNISHMENT_UNARM, LOC("PNSH_UNRM"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY, LOC("PNSH_UNRM_H"))
	{
	}

	void PunishmentUnarm::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			DedsecHack::disarm.execute(ent);
		}
	}

	PunishmentInterrupt::PunishmentInterrupt()
		: Punishment(PUNISHMENT_INTERRUPT, LOC("PNSH_INTRP"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_AIMINGAUSER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentInterrupt::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			if (!ent.isAPlayer())
			{
				ent.requestControl(ATSTRINGHASH("PunishmentInterrupt"), [](AbstractEntity& ent)
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(ent);
				});
			}
			else
			{
				ent.getPlayer().sendFreeze();
			}
		}
	}

	PunishmentFreeze::PunishmentFreeze()
		: Punishment(PUNISHMENT_FREEZE, LOC("FRZ"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentFreeze::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		DedsecHack::freeze.execute(ent);
		auto veh = ent.getVehicle(false);
		if (veh.isValid())
		{
			DedsecHack::freeze.execute(veh);
		}
	}

	PunishmentCower::PunishmentCower()
		: Punishment(PUNISHMENT_COWER, LOC("PNSH_CWR"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentCower::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			if (!TASK::GET_IS_TASK_ACTIVE(ent, CTaskTypes::TASK_COWER))
			{
				ent.requestControl(ATSTRINGHASH("cower"), [](AbstractEntity& ent)
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(ent);
					TASK::TASK_COWER(ent, -1);
				});
			}
		}
	}

	PunishmentFlee::PunishmentFlee()
		: Punishment(PUNISHMENT_FLEE, LOC("PNSH_FLEE"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentFlee::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isDead())
		{
			DedsecHack::flee.execute(ent);
		}
	}

	PunishmentPush::PunishmentPush()
		: Punishment(PUNISHMENT_PUSH, LOC("PNSH_PSH"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentPush::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		AbstractEntity target = ent;
		auto veh = target.getVehicle(false);
		if (veh.isValid())
		{
			target = veh;
		}
		if (target.isOwner())
		{
			v3 vec = g_player_ped.getPos().lookAt(target.getPos()).toDirNoZ();
			if (trigger == PunishmentTrigger::SHOOTING_AT)
			{
				vec *= 50.0f;
			}
			else
			{
				vec *= 3.0f;
			}
			__try
			{
				ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(target, APPLY_TYPE_IMPULSE, vec.x, vec.y, vec.z, true, false, true, true);
			}
			__EXCEPTIONAL()
			{
			}
		}
	}

	PunishmentDelete::PunishmentDelete()
		: Punishment(PUNISHMENT_DELETE, LOC("DEL"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_NOTBROAD | PUNISHMENTFOR_NPCHOSTILITY, NOLABEL, { CMDNAME("deletegun") })
	{
	}

	void PunishmentDelete::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (trigger > PunishmentTrigger::EXISTENCE /*&& !ENTITY::IS_ENTITY_A_MISSION_ENTITY(ent)*/)
		{
			DedsecHack::del.execute(ent);
		}
	}

	PunishmentMark::PunishmentMark()
		: Punishment(PUNISHMENT_MARK, LOC("PNSH_MRK"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentMark::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (trigger > PunishmentTrigger::EXISTENCE || !ent.isDead())
		{
			DrawUtil3d::draw_ar_marker(ent.getPos());
		}
	}

	PunishmentShowModelName::PunishmentShowModelName()
		: WorldPunishment(PUNISHMENT_SHOWMODELNAME, LOC("PNSH_SHWMDLNME"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NOTBROAD | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentShowModelName::execute(const v3& pos, AbstractEntity& _ent, PunishmentTrigger trigger) const
	{
		AbstractEntity ent = _ent;
		if (auto veh = ent.getVehicle(); veh.isValid())
		{
			ent = veh;
		}
		if (auto hash = ent.getModelHash())
		{
			if (trigger == PunishmentTrigger::AIMING_AT)
			{
				drawDebugText(joaatToString(hash));
			}
			else
			{
				Util::toast(joaatToString(hash));
			}
		}
	}

	PunishmentDrive::PunishmentDrive()
		: Punishment(PUNISHMENT_DRIVE, LOC("VEH_DRV"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_USERACTION, NOLABEL, { CMDNAME("drivegun") })
	{
	}

	void PunishmentDrive::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (ent.isPed())
		{
			auto veh = ent.getVehicle(false);
			if (veh.isValid())
			{
				DedsecHack::drive.execute(veh);
			}
		}
		else if (ent.isVehicle())
		{
			DedsecHack::drive.execute(ent);
		}
	}

	PunishmentRevive::PunishmentRevive()
		: Punishment(PUNISHMENT_REVIVE, LOC("PNSH_REV"), PUNISHMENTFOR_NPC, NOLABEL, { CMDNAME("revivegun") })
	{
	}

	void PunishmentRevive::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		ent.revive();
	}

	PunishmentRagdoll::PunishmentRagdoll()
		: Punishment(PUNISHMENT_RAGDOLL, LOC("PLYRAGDOLL"), PUNISHMENTFOR_PLAYER | PUNISHMENTFOR_NPC | PUNISHMENTFOR_NOTBROAD | PUNISHMENTFOR_NPCHOSTILITY)
	{
	}

	void PunishmentRagdoll::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		ent.ragdoll();
	}

	PunishmentRepair::PunishmentRepair()
		: Punishment(PUNISHMENT_REPAIR, LOC("PSNH_FIX"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_USERACTION)
	{
	}

	void PunishmentRepair::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		if (!ent.isVehicle())
		{
			return;
		}

		if (auto driver = ent.getDriver(); driver.isValid() && driver.isAPlayer())
		{
			driver.getPlayer().givePickupReward(ATSTRINGHASH("REWARD_VEHICLE_FIX"));
		}
		else
		{
			if (trigger == PunishmentTrigger::SHOOTING_AT)
			{
				ent.requestControl(ATSTRINGHASH("PunishmentRepair"), [](AbstractEntity& veh)
				{
					veh.wash();
					veh.repair();
				});
			}
			else // Trigger is AIMING_AT, going to be called dozens of times.
			{
				if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent))
				{
					ent.wash();
					ent.repair();
				}
				else
				{
					NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ent);
				}
			}
		}
	}

	PunishmentWeaken::PunishmentWeaken()
		: Punishment(PUNISHMENT_WEAKEN, LOC("PSNH_WEKN"), PUNISHMENTFOR_NPC | PUNISHMENTFOR_NPCHOSTILITY, LOC("PSNH_WEKN_H"))
	{
	}

	void PunishmentWeaken::execute(AbstractEntity& ent, PunishmentTrigger trigger) const
	{
		constexpr float desired_weakness = 100.0f; // Anything lower and they die from cardiac arrest like a true American.

		if (ent.getHealth() > desired_weakness)
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ent);

			if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent))
			{
				ent.setHealth(desired_weakness);
			}
		}
	}

	void Punishments::apply(punishment_t punishments, AbstractEntity& ent, PunishmentTrigger trigger)
	{
		for (const Punishment* const punishment : Punishments::all)
		{
			if (punishment->mask & punishments)
			{
				punishment->execute(ent, trigger);
			}
		}
	}

	void Punishments::apply(punishment_t punishments, const v3& pos, AbstractEntity& ent, PunishmentTrigger trigger)
	{
		for (const Punishment* const punishment : Punishments::all)
		{
			if ((punishment->mask & punishments) && (punishment->_for & PUNISHMENTFOR_WORLD))
			{
				 ((const WorldPunishment*)punishment)->execute(pos, ent, trigger);
			}
		}
	}
}
