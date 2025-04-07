#include "CommandInteriorCustomisable.hpp"

#include "CommandInterior.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"
#include "WorldState.hpp"

namespace Stand
{
	CommandInteriorCustomisable::CommandInteriorCustomisable(CommandList* const parent, std::vector<CommandName>&& command_names)
		: CommandAction(parent, LOC("TP"), std::move(command_names))
	{
	}

	void CommandInteriorCustomisable::onClick(Click& click)
	{
		ensureScriptThread(click, [this]
		{
			toggleEntitySets();
			CommandInterior::teleport(getPosition(), true);
		});
	}

	void CommandInteriorCustomisable::onOptionChanged() const
	{
		FiberPool::queueJob([this]
		{
			const auto int_id = getInteriorId();
			CommandInterior::disable(int_id);
			toggleEntitySets();
			CommandInterior::enable(int_id);
		});
	}

	void CommandInteriorCustomisable::loadFreshRandomised() const
	{
		WorldState::setOnline(TRUE);
		const auto int_id = getInteriorId();
		CommandInterior::disable(int_id);
		randomiseEntitySets();
		CommandInterior::enable(int_id);
	}

	int CommandInteriorCustomisable::getInteriorId() const
	{
		return CommandInterior::getInteriorId(getPosition());
	}

	void CommandInteriorCustomisable::toggleEntitySet(const int interior_id, const char* const name, const bool toggle)
	{
		if (toggle)
		{
			INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(interior_id, name);
		}
		else
		{
			INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(interior_id, name);
		}
	}

	void CommandInteriorCustomisable::toggleEntitySet(const char* const name, const bool toggle) const
	{
		toggleEntitySet(getInteriorId(), name, toggle);
	}
}
