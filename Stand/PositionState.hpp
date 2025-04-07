#pragma once

#include "fwddecl.hpp"
#include "natives_decl.hpp"

#include "PositionStateVehData.hpp"

namespace Stand
{
#pragma pack(push, 1)
	class PositionState
	{
	private:
		Vehicle veh;
		int seat;
		v3 pos;
		v3 rot;
		PositionStateVehData veh_data;

		PositionState(Vehicle veh, int seat, v3&& pos, v3&& rot);
	public:
		virtual ~PositionState() = default;

		[[nodiscard]] static PositionState saveSimple();
		[[nodiscard]] static PositionState save();

		[[nodiscard]] bool hasVehicle() const;

		void restore() const;
		void restoreThisTick() const;
	private:
		void restorePed() const;
		void restoreVeh(AbstractEntity& veh) const;
		void restoreEnt(AbstractEntity& ent) const;

	protected:
		virtual void onRestore() const;
	};
#pragma pack(pop)
}
