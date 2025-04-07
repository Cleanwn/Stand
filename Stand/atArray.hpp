#pragma once

#include <utility> // std::move

#include "sysMemSimpleAllocator.hpp"
#include "tlsContext.hpp"

namespace rage
{
#pragma pack(push, 8)
	template <typename T>
	class atArray
	{
	public:
		T* m_Elements = nullptr;
		uint16_t m_Count = 0;
		uint16_t m_Capacity = 0;

		atArray() = default;

		~atArray()
		{
			if (m_Elements)
			{
				rage::tlsContext::get()->sysMemAllocator_sm_Container->Free(m_Elements);
			}
		}

		T* begin() noexcept
		{
			return m_Elements;
		}

		T* end() noexcept
		{
			return m_Elements + m_Count;
		}

		const T* begin() const noexcept
		{
			return m_Elements;
		}

		const T* end() const noexcept
		{
			return m_Elements + m_Count;
		}

		T* data() noexcept
		{
			return m_Elements;
		}

		const T* data() const noexcept
		{
			return m_Elements;
		}

		uint16_t size() const noexcept
		{
			return m_Count;
		}

		uint16_t GetCount() const noexcept
		{
			return m_Count;
		}

		uint16_t capacity() const noexcept
		{
			return m_Capacity;
		}

		T& operator[](uint16_t index) noexcept
		{
			return m_Elements[index];
		}

		const T& operator[](uint16_t index) const noexcept
		{
			return m_Elements[index];
		}

		void operator=(atArray<T>&& b) noexcept
		{
			m_Elements = b.m_data;
			m_Count = b.m_size;
			m_Capacity = b.m_capacity;

			b.m_data = nullptr;
			b.m_size = 0;
			b.m_capacity = 0;
		}

		void clear()
		{
			if (m_Elements)
			{
				rage::tlsContext::get()->sysMemAllocator_sm_Container->Free(m_Elements);
				m_Elements = nullptr;
			}
			m_Count = 0;
			m_Capacity = 0;
		}

		void emplace_back(T val)
		{
			if (m_Count == m_Capacity)
			{
				reserve(m_Count + 1);
			}
			m_Elements[m_Count++] = std::move(val);
		}

		void reserve(uint16_t new_capacity)
		{
			auto new_data = (T*)rage::tlsContext::get()->sysMemAllocator_sm_Container->Allocate(new_capacity * sizeof(T), 16, 0);
			if (m_Elements != nullptr)
			{
				memcpy(new_data, m_Elements, m_Count * sizeof(T));
				rage::tlsContext::get()->sysMemAllocator_sm_Container->Free(m_Elements);
			}
			m_Elements = new_data;
			m_Capacity = new_capacity;
		}

		int Find(const T& t) const
		{
			for (int i = 0; i < m_Count; ++i)
			{
				if (m_Elements[i] == t)
				{
					return i;
				}
			}
			return -1;
		}

		void Delete(uint16_t index)
		{
			SOUP_ASSERT(index != -1);
			for (uint16_t i = index; i < m_Count - 1; ++i)
			{
				m_Elements[i] = m_Elements[i + 1];
			}
			--m_Count;
		}
	};
	static_assert(sizeof(atArray<int>) == 0x10);
#pragma pack(pop)
}
