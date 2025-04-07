#pragma once

#include "rlTaskBase.hpp"

#pragma pack(push, 1)
namespace rage
{
	struct rlHttpTask : public rlTaskBase
	{
	};

	struct rlRosHttpTask : public rlHttpTask
	{
	};

	struct rlScMatchmakingFindTask : public rlRosHttpTask
	{
		INIT_PAD(rlRosHttpTask, 0xDF0) uint32_t num_results;
		PAD(0xDF0 + 4, 0xDF8) rage::rlSessionInfo* results;
		/* 0xE00 */ uint32_t* num_results_out; // can be nullptr
	};
	static_assert(sizeof(rlScMatchmakingFindTask) == 0xE00 + 8);

	struct parTreeNode
	{
		/* 0x00 */ const char* m_ElementName;
		PAD(0x08, 0x28) parTreeNode* m_Next;
		/* 0x30 */ parTreeNode* m_Root;
		/* 0x38 */ const char* m_Data;
		/* 0x40 */ uint32_t m_HasData;
		PAD(0x44, 0x48) uint32_t m_NumChildren;

		parTreeNode* FindChild(const char* childName)
		{
			for (auto i = m_Root; i; i = i->m_Next)
			{
				if (!i)
				{
					break;
				}
				if (strcmp(childName, i->m_ElementName) == 0)
				{
					return i;
				}
			}
			return nullptr;
		}
	};
	static_assert(sizeof(parTreeNode) == 0x48 + 4);
}
#pragma pack(pop)
