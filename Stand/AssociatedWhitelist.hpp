#pragma once

namespace Stand
{
	template <typename Key, typename Value>
	class AssociatedWhitelist
	{
	private:
		std::vector<std::pair<Key, Value>> whitelist{};

	public:
		void add(Key key, Value value)
		{
			whitelist.emplace_back(std::make_pair(key, value));
		}

		void remove(Key key, Value value)
		{
			for (auto i = whitelist.begin(); i != whitelist.end(); ++i)
			{
				const auto k = i->first;
				const auto v = i->second;

				if (k == key && v == value)
				{
					whitelist.erase(i);
					break;
				}
			}
		}

		[[nodiscard]] bool contains(Value value) const
		{
			for (const auto& [k, v] : whitelist)
			{
				if (v == value)
				{
					return true;
				}
			}

			return false;
		}

		// Assumes Key is a player id.
		[[nodiscard]] playerbitset_t consumeForBitset(Value value)
		{
			playerbitset_t bitset = 0;

			for (auto i = whitelist.begin(); i != whitelist.end(); )
			{
				const auto& k = i->first;
				const auto& v = i->second;

				if (v == value)
				{
					bitset |= 1 << k;
					i = whitelist.erase(i);
				}
				else
				{
					++i;
				}
			}

			return bitset;
		}
	};
}