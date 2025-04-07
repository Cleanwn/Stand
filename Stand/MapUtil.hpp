#pragma once

namespace Stand
{
	struct MapUtil
	{
		template <typename TM, typename TK, typename TV>
		static void getWithDefault(const TM& map, const TK& key, TV&& fallback)
		{
			auto i = map.find(key);
			if (i != map.end())
			{
				return i->second;
			}
			return std::move(fallback);
		}

		template <typename TM, typename TK, typename TV>
		[[nodiscard]] static bool hasKeyValue(const TM& map, const TK& key, const TV& val)
		{
			auto i = map.find(key);
			if (i != map.end())
			{
				return i->second == val;
			}
			return false;
		}
	};
}
