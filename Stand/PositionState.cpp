#include "PositionState.hpp"

#include "AbstractEntity.hpp"
#include "atStringHash.hpp"
#include "natives.hpp"

namespace Stand
{
	PositionState::PositionState(Vehicle veh, int seat, v3&& pos, v3&& rot)
		: veh(veh), seat(seat), pos(std::move(pos)), rot(std::move(rot))
	{
	}

	PositionState PositionState::saveSimple()
	{
		Vehicle veh = 0;
		int seat = -3;
		if (g_player_veh.isValid())
		{
			veh = g_player_veh;
			seat = g_player_ped.getSeat(g_player_veh);
		}
		return PositionState(veh, seat, g_player_ped.getPos(), g_player_ped.getRot());
	}

	PositionState PositionState::save()
	{
		auto ps = PositionState::saveSimple();
		if (g_player_veh.isValid())
		{
			ps.veh_data.save(g_player_veh);
		}
		return std::move(ps);
	}

	bool PositionState::hasVehicle() const
	{
		return veh != 0;
	}

	void PositionState::restore() const
	{
		auto veh = AbstractEntity::get(this->veh);
		if (veh.exists())
		{
			veh.requestControl(ATSTRINGHASH("restore_position_state"), [this](AbstractEntity& veh)
			{
				restoreVeh(veh);
			}, [this](auto)
			{
				restorePed();
			});
		}
		else
		{
			restorePed();
		}
	}

	void PositionState::restoreThisTick() const
	{
		auto veh = AbstractEntity::get(this->veh);
		if (veh.exists() && veh.isOwner())
		{
			restoreVeh(veh);
			return;
		}
		restorePed();
	}

	void PositionState::restorePed() const
	{
		restoreEnt(g_player_ped);
		/*if (g_player_ped.is_in_any_vehicle())
		{
			TASK::CLEAR_PED_TASKS_IMMEDIATELY(g_player_ped);
		}*/
	}

	void PositionState::restoreVeh(AbstractEntity& veh) const
	{
		restoreEnt(veh);
		PED::SET_PED_INTO_VEHICLE(g_player_ped, veh, seat);
		veh_data.restore(veh);
	}

	void PositionState::restoreEnt(AbstractEntity& ent) const
	{
		//if (ent.get_pos().distanceSquared(pos) > pow2(3.0f))
		{
			ent.setPos(pos);
			ENTITY::SET_ENTITY_ROTATION(ent, rot.x, rot.y, rot.z, 2, true);
		}
		onRestore();
	}

	void PositionState::onRestore() const
	{
	}
}
