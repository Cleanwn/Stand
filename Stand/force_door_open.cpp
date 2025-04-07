#include "force_door_open.hpp"

#include "natives.hpp"

namespace Stand
{
	void force_door_open(hash_t doorHash, hash_t modelHash, float x, float y, float z)
	{
		if (doorHash == 0)
		{
			if (!OBJECT::DOOR_SYSTEM_FIND_EXISTING_DOOR(x, y, z, modelHash, &doorHash))
			{
				return;
			}
		}
		if (!OBJECT::IS_DOOR_REGISTERED_WITH_SYSTEM(doorHash))
		{
			OBJECT::ADD_DOOR_TO_SYSTEM(doorHash, modelHash, x, y, z, FALSE, FALSE, TRUE);
		}
		OBJECT::DOOR_SYSTEM_SET_DOOR_STATE(doorHash, 0, FALSE, FALSE);
	}
}
