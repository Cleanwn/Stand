#pragma once

#include "fwddecl.hpp"
#include "vector.hpp"

namespace Stand
{
	class PositionStateVehData
	{
	private:
		v3 vel;
		uint8_t wings_state;
		float wings_transform;

	public:
		void save(AbstractEntity& veh);
		void restore(AbstractEntity& veh) const;
	};
}
