#include "CommandInteriorBunker.hpp"

#include <soup/rand.hpp>

#include "CommandInterior.hpp"
#include "FiberPool.hpp"
#include "natives.hpp"

namespace Stand
{
	CommandInteriorBunker::CommandInteriorBunker(CommandList* const parent)
		: CommandInteriorCustomisable(parent, CMDNAMES("tpbunker"))
	{
		instance = this;
	}

	v3 CommandInteriorBunker::getPosition() const
	{
		return v3(900.686f, -3224.6824f, -98.27065f);
	}

	void CommandInteriorBunker::toggleEntitySets(long long style, bool security, bool equipment_upgrade, bool gun_locker, bool personal_quarters, bool gun_range) const
	{
		const int interior_id = getInteriorId();

		toggleEntitySet(interior_id, "bunker_style_a", style == 0);
		toggleEntitySet(interior_id, "bunker_style_b", style == 1);
		toggleEntitySet(interior_id, "bunker_style_c", style == 2);

		toggleEntitySet(interior_id, "standard_security_set", !security);
		toggleEntitySet(interior_id, "security_upgrade", security);

		toggleEntitySet(interior_id, "standard_bunker_set", !equipment_upgrade);
		toggleEntitySet(interior_id, "upgrade_bunker_set", equipment_upgrade);

		toggleEntitySet(interior_id, "gun_locker_upgrade", gun_locker);

		toggleEntitySet(interior_id, "Office_blocker_set", !personal_quarters);
		toggleEntitySet(interior_id, "Office_Upgrade_set", personal_quarters);

		toggleEntitySet(interior_id, "gun_range_blocker_set", !gun_range);
		toggleEntitySet(interior_id, "gun_wall_blocker", !gun_range);
		toggleEntitySet(interior_id, "gun_range_lights", gun_range);
	}

	void CommandInteriorBunker::toggleEntitySets() const
	{
		toggleEntitySets(style->value, security->m_on, equipment_upgrade->m_on, gun_locker->m_on, personal_quarters->m_on, gun_range->m_on);
	}

	void CommandInteriorBunker::randomiseEntitySets() const
	{
		toggleEntitySets(soup::rand.t<long long>(0, 2), soup::rand.coinflip(), soup::rand.coinflip(), soup::rand.coinflip(), soup::rand.coinflip(), soup::rand.coinflip());
	}
}
