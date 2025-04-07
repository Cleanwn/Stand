#pragma once

namespace rage
{
	template<typename T>
	class atSpanArray
	{
	public:
		uint8_t* m_Base;
		unsigned m_ItemSpan;
		unsigned m_Count;

		atSpanArray()
			: m_Base(nullptr), m_ItemSpan(0), m_Count(0)
		{
		}

		atSpanArray(T* base, const unsigned itemSpam, const unsigned count)
		{
			Init(base, itemSpam, count);
		}

		void Init(T* base, const unsigned itemSpam, const unsigned count)
		{
#ifdef STAND_DEBUG
			SOUP_ASSERT(itemSpam >= sizeof(T));
#endif
			m_Base = (uint8_t*)base;
			m_ItemSpan = itemSpam;
			m_Count = count;
		}
	};
}
