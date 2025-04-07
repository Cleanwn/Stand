#pragma once

namespace Stand
{
	template<typename T>
	class DistanceSort
	{
	private:
		static inline std::unordered_map<T, float> distance_cache = {};

		static inline bool sort_impl(const T& t1, const T& t2)
		{
			return distance_cache[t1] <= distance_cache[t2];
		}

	public:
		static void sort(std::vector<T>& vec, const std::function<float(T&)>&& get_distance)
		{
			for (T& t : vec)
			{
				distance_cache.emplace(t, get_distance(t));
			}
			std::sort(vec.begin(), vec.end(), sort_impl);
			distance_cache.clear();
		}
	};

	template<typename T>
	class HashedDistanceSort
	{
	private:
		static inline std::unordered_map<std::uint64_t, float> distance_cache = {};

		static inline bool sort_impl(const T& t1, const T& t2)
		{
			return distance_cache.at(t1.hash()) < distance_cache.at(t2.hash());
		}

	public:
		static void sort(std::vector<T>& vec, const std::function<float(T&)>&& get_distance)
		{
			for (T& t : vec)
			{
				distance_cache.emplace(t.hash(), get_distance(t));
			}
			std::sort(vec.begin(), vec.end(), sort_impl);
			distance_cache.clear();
		}
	};
}
