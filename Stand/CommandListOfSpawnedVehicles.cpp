#include "CommandListOfSpawnedVehicles.hpp"

#include "AbstractEntity.hpp"
#include "CommandSpawnedVehicleEntry.hpp"

namespace Stand
{
	CommandListOfSpawnedVehicles::CommandListOfSpawnedVehicles(CommandList* const parent)
		: CommandList(parent, LOC("SPWNLIST"), CMDNAMES_OBF("listofspawnedvehicles", "spawnedvehicleslist"))
	{
		instance = this;
	}

	void CommandListOfSpawnedVehicles::makeListItem(AbstractEntity& ent)
	{
		auto name = ent.getName(false);
		size_t duplicates = 1;

		instance->forEachChildAsPhysical([name, &duplicates](CommandPhysical* cmd)
		{
			if (cmd && cmd->getMenuName().getLiteralUtf8().find(name) != std::string::npos) // This is fine as long as we include manufacturer names.
			{
				++duplicates;
			}

			CONSUMER_CONTINUE;
		});

		if (duplicates > 1)
		{
			name.append(fmt::format(fmt::runtime(soup::ObfusString(" (#{})").str()), duplicates));
		}

		ent.setAlwaysSynch(true);

		instance->createChild<CommandSpawnedVehicleEntry>(LIT(name), ent);
		instance->processChildrenUpdate();
	}
}