#pragma once

#include "struct_base.hpp"

#include "fwRefAwareBase.hpp"

#pragma pack(push, 1)
namespace rage
{
	class fwExtension
	{
	public:
		virtual ~fwExtension() = default;
		virtual void _0x08() {}
		virtual void _0x10() {}
		virtual uint32_t GetExtensionId() { return 0; };
	};
	static_assert(sizeof(fwExtension) == 0x08);

	class fwExtensionList
	{
	public:
		class LinkedListNode
		{
		public:
			/* 0x00 */ fwExtension* data;
			/* 0x08 */ LinkedListNode* next;
		};
		static_assert(sizeof(LinkedListNode) == 0x10);

		/* 0x00 */ LinkedListNode* head;
		/* 0x08 */ uint32_t bitflag;
		PAD(0x08 + 4, 0x10);

		[[nodiscard]] fwExtension* Get(uint32_t id)
		{
			if (id >= 32 || ((bitflag >> id) & 1))
			{
				for (LinkedListNode* node = head; node != nullptr; node = node->next)
				{
					if (node->data->GetExtensionId() == id)
					{
						return node->data;
					}
				}
			}
			return nullptr;
		}
	};
	static_assert(sizeof(fwExtensionList) == 0x10);

	class fwExtensibleBase : public fwRefAwareBase
	{
	public:
		/* 0x10 */ fwExtensionList extensions;
	};
	static_assert(sizeof(fwExtensibleBase) == 0x20);
}
#pragma pack(pop)
