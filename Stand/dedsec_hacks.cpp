#include "dedsec_hacks.hpp"

#include "AbstractEntity.hpp"
#include "AbstractPlayer.hpp"
#include "CommandPlayer.hpp"
#include "eExplosionTag.hpp"
#include "FiberPool.hpp"
#include "Gui.hpp"
#include "is_session.hpp"
#include "atStringHash.hpp"
#include "natives.hpp"
#include "punishments.hpp"
#include "Script.hpp"
#include "eTaskType.hpp"
#include "Util.hpp"
#include "weapons.hpp"

namespace Stand
{
	constexpr DedsecHackExplode::DedsecHackExplode()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_EXP"))
	{
	}

	void DedsecHackExplode::execute(AbstractEntity& ent) const
	{
		Punishments::explode_anon.execute(ent.getPos(), ent, PunishmentTrigger::DEDSEC_HACK);
	}

	constexpr DedsecHackDestroy::DedsecHackDestroy()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_DSTRY"))
	{
	}

	void DedsecHackDestroy::execute(AbstractEntity& ent) const
	{
		ent.destroy();
	}

	constexpr DedsecHackDelete::DedsecHackDelete()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_DEL"))
	{
	}

	void DedsecHackDelete::execute(AbstractEntity& ent) const
	{
		ent.removeFromPlaneOfExistence();
	}

	constexpr DedsecHackDrive::DedsecHackDrive()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_DRV"))
	{
	}

	static uint8_t drive_counter = 0;

	void DedsecHackDrive::execute(AbstractEntity& ent) const
	{
		if (drive_counter != 0)
		{
			return;
		}
		FiberPool::queueJob([ent{ ent }]() mutable
		{
			do
			{
				if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent))
				{
					NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ent);
				}
				else
				{
					const Ped driver_ped = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ent, -1, false);
					if (driver_ped == 0)
					{
						PED::SET_PED_INTO_VEHICLE(g_player_ped, ent, -1);
						break;
					}
					if (g_player_ped.isHandleEquals(driver_ped))
					{
						break;
					}
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(driver_ped);
				}
				Script::current()->yield();
			} while (++drive_counter != 0xFF);
			drive_counter = 0;
		});

#ifdef STAND_DEBUG // Until we patch it.
		if (ent.getDriver() != g_player_ped)
		{
			TASK::TASK_ENTER_VEHICLE(g_player_ped, ent, 9999, -1, 1.0f, (1 << 1) | (1 << 3) | (1 << 4) | (1 << 9), nullptr, 0);
		}
#endif
	}

	constexpr DedsecHackEnter::DedsecHackEnter()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_ENT"))
	{
	}

	void DedsecHackEnter::execute(AbstractEntity& ent) const
	{
		const int seat = ent.getFreeSeat();
		PED::SET_PED_VEHICLE_FORCED_SEAT_USAGE(g_player_ped, ent, seat, 0, 0);
		PED::SET_PED_INTO_VEHICLE(g_player_ped, ent, seat);
	}

	constexpr DedsecHackBurn::DedsecHackBurn()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_BRN"))
	{
	}

	void DedsecHackBurn::execute(AbstractEntity& ent) const
	{
		if (!FIRE::IS_ENTITY_ON_FIRE(ent))
		{
			if (ent.getPlayerInfo() != nullptr)
			{
				if (PED::IS_PED_IN_ANY_VEHICLE(ent, true))
				{
					TASK::CLEAR_PED_TASKS_IMMEDIATELY(ent);
				}
				const v3 pos = ent.getPos();
				FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, EXP_TAG_MOLOTOV, 1.0f, false, false, 0.0f, false);
			}
			else
			{
				ent.requestControl(ATSTRINGHASH("burn"), [](AbstractEntity& ped)
				{
					ENTITY::SET_ENTITY_PROOFS(ped, false, false, false, false, false, false, false, false);
					FIRE::START_ENTITY_FIRE(ped);
				});
			}
		}
	}

	constexpr DedsecHackRevive::DedsecHackRevive()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_REV"))
	{
	}

	void DedsecHackRevive::execute(AbstractEntity& ent) const
	{
		ent.requestControl(ATSTRINGHASH("revive"), [](AbstractEntity& ent)
		{
			ENTITY::SET_ENTITY_HEALTH(ent, ENTITY::GET_ENTITY_MAX_HEALTH(ent), 0, 0);
			if (FIRE::IS_ENTITY_ON_FIRE(ent))
			{
				FIRE::STOP_ENTITY_FIRE(ent);
			}
			TASK::CLEAR_PED_TASKS_IMMEDIATELY(ent);
		});
	}

	constexpr DedsecHackMenuPlayer::DedsecHackMenuPlayer()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_MNU"))
	{
	}

	void DedsecHackMenuPlayer::execute(AbstractEntity& ent) const
	{
		CommandPlayer* player_command = ent.getPlayer().getCommand();
		Click click(CLICK_AUTO, TC_SCRIPT_NOYIELD);
		player_command->onClick(click);
	}

	constexpr DedsecHackMenuPlayerVeh::DedsecHackMenuPlayerVeh()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_MNU"))
	{
	}

	void DedsecHackMenuPlayerVeh::execute(AbstractEntity& ent) const
	{
		auto driver = ent.getDriver();
		menu_player.execute(driver);
	}

	constexpr DedsecHackFreeze::DedsecHackFreeze()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_FRZ"))
	{
	}

	void DedsecHackFreeze::execute(AbstractEntity& ent) const
	{
		ent.freeze();
	}

	constexpr DedsecHackUnfreeze::DedsecHackUnfreeze()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_UFRZ"))
	{
	}

	void DedsecHackUnfreeze::execute(AbstractEntity& ent) const
	{
		ent.unfreeze();
	}

	constexpr DedsecHackEmpty::DedsecHackEmpty()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_EMPTY"))
	{
	}

	void DedsecHackEmpty::execute(AbstractEntity& ent) const
	{
		ent.makePassengersLeave();
	}

	constexpr DedsecHackDisarm::DedsecHackDisarm()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_UNRM"))
	{
	}

	void DedsecHackDisarm::execute(AbstractEntity& ent) const
	{
		ent.removeAllWeapons();
	}

	constexpr DedsecHackCower::DedsecHackCower()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_CWR"))
	{
	}

	void DedsecHackCower::execute(AbstractEntity& ent) const
	{
		TASK::TASK_COWER(ent, -1);
	}

	constexpr DedsecHackFlee::DedsecHackFlee()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_FLEE"))
	{
	}

	void DedsecHackFlee::execute(AbstractEntity& ent) const
	{
		if (!TASK::GET_IS_TASK_ACTIVE(ent, CTaskTypes::TASK_SMART_FLEE))
		{
			ent.requestControl(ATSTRINGHASH("flee"), [](AbstractEntity& ent)
			{
				ent.unfreeze();
				TASK::TASK_SMART_FLEE_PED(ent, g_player_ped, FLT_MAX, -1, true, true);
			});
		}
	}

	constexpr DedsecHackIgnite::DedsecHackIgnite()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_IGNITE"), false)
	{
	}

	void DedsecHackIgnite::execute(AbstractEntity& ent) const
	{
		ent.requestControl(ATSTRINGHASH("ignite"), [](AbstractEntity& veh)
		{
			VEHICLE::SET_VEHICLE_PETROL_TANK_HEALTH(veh, -1.0f);
		});
	}

	constexpr DedsecHackSlingshot::DedsecHackSlingshot()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_SLINGSHOT"))
	{
	}

	void DedsecHackSlingshot::execute(AbstractEntity& ent) const
	{
		ent.requestControl(ATSTRINGHASH("slingshot"), [](AbstractEntity& veh)
		{
			veh.slingshot();
		});
	}

	constexpr DedsecHackCage::DedsecHackCage()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_CGE"), false)
	{
	}

	void DedsecHackCage::execute(AbstractEntity& ent) const
	{
		ent.cage();
	}

	constexpr DedsecHackKill::DedsecHackKill()
		: DedsecHack(ATSTRINGHASH("DEDSEC_OPT_KLL"), false)
	{
	}

	void DedsecHackKill::execute(AbstractEntity& ent) const
	{
		if (ent.isAPlayer())
		{
			ent.getPlayer().kill();
		}
	}

	DedsecHackExplode DedsecHack::explode = DedsecHackExplode();
	DedsecHackDestroy DedsecHack::destroy = DedsecHackDestroy();
	DedsecHackDelete DedsecHack::del = DedsecHackDelete();
	DedsecHackDrive DedsecHack::drive = DedsecHackDrive();
	DedsecHackEnter DedsecHack::enter = DedsecHackEnter();
	DedsecHackBurn DedsecHack::burn = DedsecHackBurn();
	DedsecHackRevive DedsecHack::revive = DedsecHackRevive();
	DedsecHackMenuPlayer DedsecHack::menu_player = DedsecHackMenuPlayer();
	DedsecHackMenuPlayerVeh DedsecHack::menu_player_veh = DedsecHackMenuPlayerVeh();
	DedsecHackFreeze DedsecHack::freeze = DedsecHackFreeze();
	DedsecHackUnfreeze DedsecHack::unfreeze = DedsecHackUnfreeze();
	DedsecHackEmpty DedsecHack::empty = DedsecHackEmpty();
	DedsecHackDisarm DedsecHack::disarm = DedsecHackDisarm();
	DedsecHackCower DedsecHack::cower = DedsecHackCower();
	DedsecHackFlee DedsecHack::flee = DedsecHackFlee();
	DedsecHackIgnite DedsecHack::ignite = DedsecHackIgnite();
	DedsecHackSlingshot DedsecHack::slingshot = DedsecHackSlingshot();
	DedsecHackCage DedsecHack::cage = DedsecHackCage();
	DedsecHackKill DedsecHack::kill = DedsecHackKill();
}
