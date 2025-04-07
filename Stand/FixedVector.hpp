#pragma once

#include <atomic>

namespace Stand
{
	template <typename T, size_t Capacity>
	struct FixedVector // AtomicArray?
	{
		std::atomic<T> data[Capacity]{};

		[[nodiscard]] bool empty() const
		{
			for (size_t i = 0; i != Capacity; ++i)
			{
				if (data[i].load())
				{
					return false;
				}
			}
			return true;
		}

		[[nodiscard]] size_t count() const
		{
			size_t count = 0;
			for (size_t i = 0; i != Capacity; ++i)
			{
				if (data[i].load())
				{
					++count;
				}
			}
			return count;
		}

		[[nodiscard]] size_t countExcept(const T& x) const
		{
			size_t count = 0;
			for (size_t i = 0; i != Capacity; ++i)
			{
				const T val = data[i].load();
				if (val && val != x)
				{
					++count;
				}
			}
			return count;
		}

		[[nodiscard]] bool contains(const T& val) const
		{
			for (size_t i = 0; i != Capacity; ++i)
			{
				if (data[i] == val)
				{
					return true;
				}
			}
			return false;
		}

		void emplace(const T& val)
		{
			for (size_t i = 0; i != Capacity; ++i)
			{
				T expected{};
				if (data[i].compare_exchange_strong(expected, val))
				{
					return;
				}
			}
			throw 0;
		}

		bool erase(const T& val)
		{
			if (val != T{}) // Avoid infinite loop e.g. if someone does `while (FixedVector::erase(x));` and x equals our no-value signal.
			{
				[[likely]];
				for (size_t i = 0; i != Capacity; ++i)
				{
					if (data[i].load() == val)
					{
						data[i].store(T{});
						return true;
					}
				}
			}
			return false;
		}
	};
}
