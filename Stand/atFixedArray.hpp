#pragma once

namespace rage
{
	template <typename T, size_t S>
	struct atFixedArray
	{
		T elements[S];
		int m_Count;

		[[nodiscard]] static constexpr int capacity() noexcept
		{
			return S;
		}

		T& operator[](int index) noexcept
		{
			return elements[index];
		}

		const T& operator[](int index) const noexcept
		{
			return elements[index];
		}

		int Find(const T& t) const noexcept
		{
			for (int i = 0; i != m_Count; ++i)
			{
				if (elements[i] == t)
				{
					return i;
				}
			}
			return -1;
		}

		void Delete(int index) noexcept
		{
			for (int i = index; i < m_Count - 1; i++)
			{
				elements[i] = elements[i + 1];
			}
			--m_Count;
		}

		[[nodiscard]] bool canAppend() const noexcept
		{
			return m_Count != capacity() - 1;
		}

		T& Append()
		{
			return elements[m_Count++];
		}

		void Push(const T& t)
		{
			Append() = t;
		}
	};
}
