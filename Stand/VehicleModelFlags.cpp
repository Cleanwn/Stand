#include "VehicleModelFlags.hpp"

#include "Hooking.hpp"

namespace Stand
{
	VehicleModelFlags::VehicleModelFlags(const soup::BigBitset<25>* flags)
	{
		memcpy(&this->flags, flags, sizeof(this->flags));
	}

	void VehicleModelFlags::set(CVehicleModelInfo* info, CVehicleModelInfo::Flags flag, bool value)
	{
		if (info->flags.get(flag) != value)
		{
			if (!modified.contains(info->hash))
			{
				modified.emplace(info->hash);

				if (originals.find(info->hash) == originals.end())
				{
					originals.emplace(info->hash, &info->flags);
				}
			}
			info->flags.set(flag, value);
		}
	}

	void VehicleModelFlags::reset(CVehicleModelInfo* info, CVehicleModelInfo::Flags flag, bool no_erase)
	{
		auto og_e = originals.find(info->hash);
		if (og_e != originals.end())
		{
			const bool og_value = og_e->second.flags.get(flag);
			if (no_erase)
			{
				info->flags.set(flag, og_value);
			}
			else if (info->flags.get(flag) != og_value)
			{
				info->flags.set(flag, og_value);
				if (memcmp(&info->flags, &og_e->second.flags, sizeof(og_e->second.flags)) == 0)
				{
					modified.erase(info->hash);
				}
			}
		}
	}

	void VehicleModelFlags::reset(CVehicleModelInfo::Flags flag)
	{
		for (auto it = modified.begin(); it != modified.end(); )
		{
			unsigned int idx = 0xFFFF;
			auto info = OG(rage_fwArchetypeManager_GetArchetypeFromHashKey)(*it, &idx);
			if (idx == 0xFFFF)
			{
				it = modified.erase(it);
			}
			else
			{
				reset((CVehicleModelInfo*)info, flag, true);
				++it;
			}
		}
	}

	void VehicleModelFlags::resetAll()
	{
		for (const auto& model : modified)
		{
			auto og_e = originals.find(model);
			if (og_e != originals.end())
			{
				unsigned int idx = 0xFFFF;
				auto info = OG(rage_fwArchetypeManager_GetArchetypeFromHashKey)(model, &idx);
				if (idx != 0xFFFF)
				{
					memcpy(&((CVehicleModelInfo*)info)->flags, &og_e->second.flags, sizeof(og_e->second.flags));
				}
			}
		}
		modified.clear();
	}
}
