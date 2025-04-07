#include "gta_entity.hpp"

#include "pointers.hpp"

#ifdef STAND_DEBUG
#include "eEntityType.hpp"
#include "gta_vehicle.hpp"
#endif

namespace rage
{
#ifdef STAND_DEBUG
	std::string fwEntity::toString() const
	{
		switch (type)
		{
		case ENTITY_TYPE_VEHICLE:
			return fmt::format("ENTITY_TYPE_VEHICLE ({})", (int)static_cast<const CVehicle*>(this)->vehicle_type);
		}
		return fmt::to_string(type);
	}
#endif

	fwEntity* fwEntity::GetAttachedTo()
	{
		return Stand::pointers::rage_fwEntity_GetAttachedTo(this);
	}
}

const rage::phArchetypeDamp* CEntity::GetPhysArch() const noexcept
{
	if (ph_inst
		&& ph_inst->archetype
		)
	{
		// Can't static_cast because compiler does not know inheritance
		return reinterpret_cast<rage::phArchetypeDamp*>(ph_inst->archetype);
	}
	return archetype->m_DynamicComponent->m_pPhysicsArch;
}
