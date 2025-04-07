#include "PointerCache.hpp"

#include <soup/FileReader.hpp>
#include <soup/FileWriter.hpp>

#include "get_appdata_path.hpp"
#include "StringUtils.hpp"

namespace Stand
{
	bool PointerCache::isInited()
	{
		return !file.empty();
	}

	void PointerCache::init()
	{
		PointerCache::file = get_appdata_path(soup::ObfusString("Cache\\"));
		if (!std::filesystem::exists(PointerCache::file))
		{
			std::filesystem::create_directory(PointerCache::file);
		}
		PointerCache::file.append(StringUtils::utf8_to_utf16(soup::ObfusString("Offsets.bin")));
		soup::FileReader cache(PointerCache::file);
		while (cache.hasMore())
		{
			std::string name;
			uintptr_t offset;
			if (!cache.str_nt(name)
				|| !cache.u64_dyn(offset)
				)
			{
				break;
			}
			PointerCache::cache.emplace(std::move(name), offset ^ PointerCache::xor_magic);
		}
	}

	void PointerCache::save()
	{
#ifdef STAND_DEBUG
		SOUP_ASSERT(isInited());
#endif
		soup::FileWriter cache(PointerCache::file);
		for (const auto& entry : PointerCache::cache)
		{
			if (!cache.str_nt(entry.first)
				|| !cache.u64_dyn(entry.second ^ PointerCache::xor_magic)
				)
			{
				break;
			}
		}
	}

	void PointerCache::deinit()
	{
		file.clear();
		cache.clear();
	}

	void PointerCache::saveAndDeinit()
	{
		save();
		deinit();
	}
}
