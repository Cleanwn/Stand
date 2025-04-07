#pragma once

#include "struct_base.hpp"

namespace rage
{
	template <typename T>
	struct inlist_node
	{
		/* 0x00 */ T* m_next;
		/* 0x08 */ T* m_prev;
	};
	static_assert(sizeof(inlist_node<void>) == 0x10);

	template <typename T, inlist_node<T> T::*NODE>
	struct inlist
	{
		/* 0x00 */ T* m_head;
		/* 0x08 */ T* m_tail;
		/* 0x10 */ size_t m_size;
	}; // size = 0x18
}
