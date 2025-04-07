#include "CommandObjectSpawner.hpp"

#include "joaat_hash_db.hpp"

#include "CommandDivider.hpp"
#include "CommandFindObject.hpp"
#include "CommandSpawnAnyObject.hpp"
#include "CommandSpawnObject.hpp"

namespace Stand
{
	CommandObjectSpawner::CommandObjectSpawner(CommandList* parent)
		: CommandList(parent, LOC("SPWNR"))
	{
		this->createChild<CommandFindObject>();
		this->createChild<CommandSpawnAnyObject>(parent);
		this->createChild<CommandDivider>(LOC("OBJS"));
		objects_offset = this->children.size();
		{
			auto downtown = createChild<CommandList>(LOC("DWNTWN"));
			for (const auto& object : g_objects_downtown)
			{
				downtown->createChild<CommandSpawnObject>(parent, object);
			}
		}
		for (const auto& object : g_objects)
		{
			this->createChild<CommandSpawnObject>(parent, object);
		}
	}
}
