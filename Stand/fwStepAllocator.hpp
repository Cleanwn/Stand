#pragma once

namespace rage
{
	class fwStepAllocator
	{
	private:
		uint8_t* m_Ptr;
		size_t m_SizeLeft;

	public:
		fwStepAllocator() { m_Ptr = 0; m_SizeLeft = 0; }
		fwStepAllocator(void* ptr, uint32_t size) { Init(ptr, size); }

		void Init(void* ptr, uint32_t size)
		{
			m_Ptr = (uint8_t*)ptr;
			m_SizeLeft = size;
		}

		void* Alloc(uint32_t size)
		{
			if (size <= m_SizeLeft)
			{
				uint8_t* p = m_Ptr;
				m_Ptr += size;
				m_SizeLeft -= size;
				return p;
			}

			return nullptr;
		}

		void* TryAlloc(uint32_t size)
		{
			if (size <= m_SizeLeft)
			{
				uint8_t* p = m_Ptr;
				m_Ptr += size;
				m_SizeLeft -= size;
				return p;
			}

			return nullptr;
		}

		void* Alloc(uint32_t size, uint32_t align)
		{
			const size_t fillup = ((size_t(m_Ptr) + size_t(align - 1)) & ~size_t(align - 1)) - size_t(m_Ptr);	// alignment fillup

			if ((size + fillup) <= m_SizeLeft)
			{
				m_Ptr += fillup;
				m_SizeLeft -= fillup;
				return Alloc(size);
			}

			return nullptr;
		}

		template <typename Type>
		Type* Alloc(const uint32_t count, const uint32_t align) {
			return reinterpret_cast<Type*>(Alloc(sizeof(Type) * count, align));
		}

		template <typename Type>
		Type* Alloc(const uint32_t count) {
			return reinterpret_cast<Type*>(Alloc(sizeof(Type) * count));
		}
	};
}
