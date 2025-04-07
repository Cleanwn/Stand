#pragma once

namespace Stand
{
	template <typename T>
	class Whitelist
	{
	private:
		std::vector<T> list{};

	public:
		[[nodiscard]] bool isActive() const noexcept
		{
			return !list.empty();
		}

		void add(T&& e)
		{
			list.emplace_back(std::move(e));
		}

		void remove(const T& e) noexcept
		{
			list.erase(std::find(list.begin(), list.end(), e));
		}

		[[nodiscard]] bool contains(const T& e) noexcept
		{
			return std::find(list.begin(), list.end(), e) != list.end();
		}

		void reset() noexcept
		{
			list.clear();
		}
	};
}
