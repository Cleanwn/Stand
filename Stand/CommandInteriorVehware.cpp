#include "CommandInteriorVehware.hpp"

#include <soup/rand.hpp>

#include "CommandInterior.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"

namespace Stand
{
	CommandInteriorVehware::CommandInteriorVehware(CommandList* parent)
		: CommandInteriorCustomisable(parent, CMDNAMES("tpvehware"))
	{
		instance = this;
	}

	v3 CommandInteriorVehware::getPosition() const
	{
		return v3(975.0267, -3000.2334, -39.648537);
	}

	void CommandInteriorVehware::toggleEntitySets(long long style) const
	{
		const int interior_id = getInteriorId();

		toggleEntitySet(interior_id, "Basic_style_set", style <= 0);
		toggleEntitySet(interior_id, "Urban_style_set", style == 1);
		toggleEntitySet(interior_id, "Branded_style_set", style == 2);

		if (style <= 0)
		{
			INTERIOR::SET_INTERIOR_ENTITY_SET_TINT_INDEX(interior_id, "Basic_style_set", int(style) * -1);
		}
	}

	void CommandInteriorVehware::toggleEntitySets() const
	{
		toggleEntitySets(style->value);
	}

	void CommandInteriorVehware::randomiseEntitySets() const
	{
		auto style = soup::rand.t<long long>(0, 2);
		if (style == 0)
		{
			style = soup::rand.t<long long>(-3, 0);
		}
		toggleEntitySets(style);
	}
}
