#include "CommandListMyVehicle.hpp"

#include "AbstractEntity.hpp"

#include "CommandCall.hpp"
#include "CommandEnter.hpp"

namespace Stand
{
	Vehicle CommandListMyVehicle::getPersonalVehicle()
	{
		return AbstractEntity::getPersonalVehicle();
	}

	Vehicle CommandListMyVehicle::getLastVehicle()
	{
		return g_player_ped.getVehicle(true);
	}

	CommandListMyVehicle::CommandListMyVehicle(CommandList* const parent, bool pvehicle) :
		CommandList(parent, pvehicle ? LOC("CURPV") : LOC("LSTVEH"),
		pvehicle ? CMDNAMES("personalvehicle", "pvehicle") : CMDNAMES("lastvehicle")),
		getVehicleInfo(pvehicle ? GetVehicleInfo{ ATSTRINGHASH("PVEHNTFND"), &getPersonalVehicle } : GetVehicleInfo{ ATSTRINGHASH("LSTVEHNTFND"), &getLastVehicle })
	{
		this->createChild<CommandCall>(&getVehicleInfo, pvehicle ? CMDNAMES("callpersonalvehicle", "callpvehicle") : CMDNAMES("calllastvehicle"));
		this->createChild<CommandEnter>(&getVehicleInfo, pvehicle ? CMDNAMES("enterpersonalvehicle", "enterpvehicle", "drivepvehicle") : CMDNAMES("enterlastvehicle", "drivelastvehicle"));
	}
}
