#pragma once

#include "datBase.hpp"

namespace rage
{
	struct fwKnownRefHolder
	{
		/* 0x00 */ void** m_ppReference;
		/* 0x08 */ fwKnownRefHolder* m_pNext;
	};

	template <typename T>
	class fwRefAwareBaseImpl : public T
	{
	public:
		/* 0x08 */ fwKnownRefHolder* m_pKnownRefHolderHead;
	};

	class fwRefAwareBase : public fwRefAwareBaseImpl<datBase>
	{
	public:
		void RemoveKnownRef(void** ppReference);
	};
	static_assert(sizeof(fwRefAwareBase) == 0x10);
}
