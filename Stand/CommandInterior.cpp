#include "CommandInterior.hpp"

#include "Vector2Plus.hpp"
#include "WorldState.hpp"

namespace Stand
{
	CommandInterior::CommandInterior(CommandList* const parent, Label&& menu_name, std::vector<CommandName>&& command_names, v3&& pos, const bool needs_mp_world_state)
		: CommandAction(parent, std::move(menu_name), std::move(command_names), needs_mp_world_state ? LOC("TP_MP") : NOLABEL), pos(std::move(pos)), needs_mp_world_state(needs_mp_world_state)
	{
	}

	CommandInterior::CommandInterior(CommandList* const parent, Label&& menu_name, v3&& pos, const bool needs_mp_world_state)
		: CommandInterior(parent, std::move(menu_name), {}, std::move(pos), needs_mp_world_state)
	{
	}

	int CommandInterior::getInteriorId() const
	{
		return getInteriorId(pos);
	}

	int CommandInterior::getInteriorId(const v3& pos)
	{
		return INTERIOR::GET_INTERIOR_AT_COORDS(pos.x, pos.y, pos.z);
	}

	void CommandInterior::disable(const int interior_id)
	{
		if (!INTERIOR::IS_INTERIOR_CAPPED(interior_id))
		{
			INTERIOR::CAP_INTERIOR(interior_id, TRUE);
		}
		if (!INTERIOR::IS_INTERIOR_DISABLED(interior_id))
		{
			INTERIOR::DISABLE_INTERIOR(interior_id, TRUE);
		}
		INTERIOR::UNPIN_INTERIOR(interior_id);
	}

	void CommandInterior::disable() const
	{
		disable(getInteriorId());
	}

	void CommandInterior::enable() const
	{
		enable(pos, needs_mp_world_state);
	}

	void CommandInterior::enable(const v3& pos, const bool needs_mp_world_state)
	{
		if (needs_mp_world_state)
		{
			WorldState::setOnline(TRUE);
		}
		if (auto int_id = getInteriorId(pos))
		{
			enable(int_id);
		}
	}

	void CommandInterior::enable(const int interior_id)
	{
		INTERIOR::PIN_INTERIOR_IN_MEMORY(interior_id);
		if (INTERIOR::IS_INTERIOR_CAPPED(interior_id))
		{
			INTERIOR::CAP_INTERIOR(interior_id, FALSE);
		}
		if (INTERIOR::IS_INTERIOR_DISABLED(interior_id))
		{
			INTERIOR::DISABLE_INTERIOR(interior_id, FALSE);
		}
		INTERIOR::REFRESH_INTERIOR(interior_id);
	}

	void CommandInterior::teleport() const
	{
		enable();
		TpUtil::teleport(pos);
	}

	void CommandInterior::teleport(const v3& pos, const bool needs_mp_world_state)
	{
		enable(pos, needs_mp_world_state);
		TpUtil::teleport(pos);
	}

	void CommandInterior::onClick(Click& click)
	{
		ensureScriptThread(click, [this]
		{
			teleport();
		});
	}
}
