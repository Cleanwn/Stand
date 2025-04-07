#include "CrossState.hpp"

#include "CNetworkSession.hpp"
#include "pointers.hpp"

namespace Stand
{
	CrossState*& CrossState::ptr()
	{
		return *reinterpret_cast<CrossState**>(&(*pointers::network_session)->session->unused_memory);
	}

	CrossState::CrossState()
		: version_major(current_version_major), version_minor(current_version_minor)
	{
	}

	CrossState& CrossState::get()
	{
		if (ptr() == nullptr)
		{
			ptr() = new CrossState();
		}
		return *ptr();
	}

	CustomVehicleModel*& CrossState::getCustomVehicleModelsTail()
	{
		auto pNode = &custom_vehicle_models_head;
		while (*pNode && (*pNode)->next)
		{
			pNode = &(*pNode)->next;
		}
		return *pNode;
	}
}
