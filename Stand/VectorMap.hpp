#pragma once

#include <unordered_map>
#include <vector>

namespace Stand
{
	template <typename K, typename V>
	class VectorMap
	{
	private:
		using VecT = std::vector<V>;
		using MapT = std::unordered_map<K, VecT>;

		MapT map{};

	public:
		[[nodiscard]] typename MapT::iterator begin() noexcept
		{
			return map.begin();
		}

		[[nodiscard]] typename MapT::iterator end() noexcept
		{
			return map.end();
		}

		[[nodiscard]] typename MapT::const_iterator cbegin() const noexcept
		{
			return map.cbegin();
		}

		[[nodiscard]] typename MapT::const_iterator cend() const noexcept
		{
			return map.cend();
		}

		[[nodiscard]] VecT& at(const K& k)
		{
			return map.at(k);
		}

		[[nodiscard]] const VecT& at(const K& k) const
		{
			return map.at(k);
		}

		[[nodiscard]] typename MapT::iterator find(const K& k)
		{
			return map.find(k);
		}

		[[nodiscard]] bool contains(const K& k)
		{
			return find(k) != end();
		}

		void emplace(K&& k, V&& v)
		{
			auto e = find(k);
			if (e == end())
			{
				VecT vec{};
				vec.emplace_back(std::move(v));
				map.emplace(std::move(k), std::move(vec));
			}
			else
			{
				e->second.emplace_back(std::move(v));
			}
		}

		void erase(const K& k) noexcept
		{
			map.erase(k);
		}

		auto erase(typename MapT::const_iterator it) noexcept
		{
			return map.erase(it);
		}

		[[nodiscard]] size_t size() const noexcept
		{
			return map.size();
		}

		void clear() noexcept
		{
			map.clear();
		}

		// You may think the following implementations look overcomplicated, and you're correct, but for some reason, MSVC only likes ++ on map iterators, not +.

		[[nodiscard]] VecT* at(size_t idx) noexcept
		{
			size_t i = 0;
			for (auto& e : map)
			{
				if (i == idx)
				{
					return &e.second;
				}
				++i;
			}
			return nullptr;
		}

		[[nodiscard]] const VecT* at(size_t idx) const noexcept
		{
			size_t i = 0;
			for (const auto& e : map)
			{
				if (i == idx)
				{
					return &e.second;
				}
				++i;
			}
			return nullptr;
		}

		void erase(size_t idx) noexcept
		{
			size_t i = 0;
			for (auto it = map.cbegin(); it != map.cend(); ++it)
			{
				if (i == idx)
				{
					map.erase(it);
					return;
				}
				++i;
			}
		}
	};
}
