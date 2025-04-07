#include "PersonalVehicle.hpp"

#include <fmt/core.h>

#include <soup/Bitset.hpp>

#include "atStringHash.hpp"
#include "natives.hpp"

namespace Stand
{
	hash_t PersonalVehicle::getStatHash(const char* key) const
	{
		return rage::atStringHash(fmt::format("mp{}_mpsv_{}_{}", ScriptGlobal(GLOBAL_CHAR_SLOT).get<int>(), key, slot));
	}

	bool PersonalVehicle::getFlag(int bit) const noexcept
	{
		return getBase().at(GLOBAL_PVS_FLAGS).as<soup::Bitset<int>*>()->get(bit);
	}

	void PersonalVehicle::setFlag(int bit, bool val) const
	{
		if (getFlag(bit) != val)
		{
			getBase().at(GLOBAL_PVS_FLAGS).as<soup::Bitset<int>*>()->set(bit, val);
			writeFlags();
		}
	}

	void PersonalVehicle::writeFlags() const
	{
		STATS::STAT_SET_INT(getStatHash("flags"), getBase().at(GLOBAL_PVS_FLAGS).get<int>(), true);
	}

	bool PersonalVehicle::getInsuranceFlag(int bit) const noexcept
	{
		return getBase().at(GLOBAL_PVS_INSURANCE).as<soup::Bitset<int>*>()->get(bit);
	}

	void PersonalVehicle::setInsuranceFlag(int bit, bool val) const
	{
		if (getInsuranceFlag(bit) != val)
		{
			getBase().at(GLOBAL_PVS_INSURANCE).as<soup::Bitset<int>*>()->set(bit, val);
		}
	}

	void PersonalVehicle::writeInsuranceFlags() const
	{
		STATS::STAT_SET_INT(getStatHash("bs"), getBase().at(GLOBAL_PVS_INSURANCE).get<int>(), true);
	}

#define GLOBAL_STAT_SAVE_SLOT_TYPES									2725118
#define GLOBAL_STAT_SAVE_SLOT_HASHES								2725129

	void PersonalVehicle::saveStats()
	{
#if false
		int slot = 9;
		for (int i = 0; i <= 9; ++i)
		{
			if (ScriptGlobal(GLOBAL_STAT_SAVE_SLOT_TYPES).at(i, 1).get<int>() == 0)
			{
				slot = i;
				break;
			}
		}
		ScriptGlobal(GLOBAL_STAT_SAVE_SLOT_TYPES).at(slot, 1).set<int>(3);
		ScriptGlobal(GLOBAL_STAT_SAVE_SLOT_HASHES).at(slot, 1).set<Hash>(-1948815871);
		//script_global(GLOBAL_STAT_SAVE_SLOT_HASHES).at(slot, 1).set<Hash>(1354987415);
		//script_global(GLOBAL_STAT_SAVE_SLOT_HASHES).at(slot, 1).set<Hash>(1013997354);
#endif
	}
}
