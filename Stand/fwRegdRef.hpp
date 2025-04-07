#pragma once

namespace rage
{
	template <typename T>
	struct fwRegdRef
	{
		T* m_p;

		operator T* () const noexcept
		{
			return m_p;
		}

		T* operator ->() const noexcept
		{
			return m_p;
		}

		// Set this ref to nullptr.
		void reset()
		{
			if (m_p != nullptr)
			{
				static_cast<fwRefAwareBase*>(m_p)->RemoveKnownRef((void**)&m_p);
				m_p = nullptr;
			}
		}
	};
}
