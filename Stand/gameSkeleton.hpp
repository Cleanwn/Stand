#pragma once

#include "struct_base.hpp"

#pragma pack(push, 1)
namespace rage
{
	struct gameSkeleton
	{
		typedef void (*fnUpdateFunction) ();

		struct updateBase
		{
			virtual ~updateBase() = default;
			virtual void Update() = 0;

			PAD(0x08, 0x10) uhash_t m_Name;
			PAD(0x14, 0x18) updateBase* m_Next;
		};
		static_assert(sizeof(updateBase) == 0x20);

		struct updateElement : public updateBase
		{
			/* 0x20 */ fnUpdateFunction m_UpdateFunction;
		};
		static_assert(sizeof(updateElement) == 0x28);

		struct updateGroup : public updateBase
		{
			/* 0x20 */ updateBase* m_Head;

			void Update()
			{
				for (updateBase* element = m_Head; element; element = element->m_Next)
				{
					element->Update();
				}
			}
		};
		static_assert(sizeof(updateGroup) == 0x28);

		struct updateMode
		{
			/* 0x00 */ uint32_t m_ModeType;
			PAD(0x04, 0x08) updateBase* m_Head;
			/* 0x10 */ updateMode* m_Next;

			void Update()
			{
				for (updateBase* element = m_Head; element; element = element->m_Next)
				{
					element->Update();
				}
			}
		};
		static_assert(sizeof(updateMode) == 0x18);

		virtual ~gameSkeleton() = default;

		PAD(0x008, 0x140) updateMode* m_UpdateModes;

		void Update(uint32_t updateModeType)
		{
			for (updateMode* mode = m_UpdateModes; mode; mode = mode->m_Next)
			{
				if (mode->m_ModeType == updateModeType)
				{
					return mode->Update();
				}
			}
		}

		[[nodiscard]] updateMode* findUpdateMode(uint32_t updateModeType) const noexcept
		{
			for (updateMode* mode = m_UpdateModes; mode; mode = mode->m_Next)
			{
				if (mode->m_ModeType == updateModeType)
				{
					return mode;
				}
			}
			return nullptr;
		}

		[[nodiscard]] updateElement* findUpdateElement(uint32_t name) noexcept
		{
			for (auto* mode = this->m_UpdateModes; mode; mode = mode->m_Next)
			{
				for (auto* update_node = mode->m_Head; update_node; update_node = update_node->m_Next)
				{
					if (update_node->m_Name == ATSTRINGHASH("Common Main"))
					{
						auto* group = static_cast<rage::gameSkeleton::updateGroup*>(update_node);
						for (auto* group_child_node = group->m_Head; group_child_node; group_child_node = group_child_node->m_Next)
						{
							if (group_child_node->m_Name == name)
							{
								return static_cast<rage::gameSkeleton::updateElement*>(group_child_node);
							}
						}
						break;
					}
				}
			}
			return nullptr;
		}
	};
	static_assert(sizeof(gameSkeleton) == 0x140 + 8);
}
#pragma pack(pop)
