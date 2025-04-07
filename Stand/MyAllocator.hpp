#pragma once

namespace Stand
{
	struct Allocation
	{
		void* addr;
		size_t size;
	};

	template <size_t BlockSize, size_t MaxAllocsPerBlock, size_t MaxBlocks>
	struct MyAllocator
	{
		struct BlockInfo
		{
			Allocation allocs[MaxAllocsPerBlock];

			[[nodiscard]] bool isEmpty() const noexcept
			{
				for (auto& alloc : allocs)
				{
					if (alloc.addr != nullptr)
					{
						return false;
					}
				}
				return true;
			}

			[[nodiscard]] Allocation* getFreeAllocationSlot() noexcept
			{
				for (auto& alloc : allocs)
				{
					if (alloc.addr == nullptr)
					{
						return &alloc;
					}
				}
				return nullptr;
			}

			[[nodiscard]] Allocation* getAllocationAt(void* addr) noexcept
			{
				for (auto& alloc : allocs)
				{
					if (alloc.addr == addr)
					{
						return &alloc;
					}
				}
				return nullptr;
			}

			[[nodiscard]] const Allocation* getAllocationAt(void* addr) const noexcept
			{
				for (const auto& alloc : allocs)
				{
					if (alloc.addr == addr)
					{
						return &alloc;
					}
				}
				return nullptr;
			}

			[[nodiscard]] const Allocation* getNextAllocation(void* addr) const noexcept
			{
				const Allocation* next_alloc = nullptr;
				uintptr_t min_dist = -1;
				for (const auto& alloc : allocs)
				{
					if (alloc.addr > addr)
					{
						uintptr_t dist = reinterpret_cast<uintptr_t>(alloc.addr) - reinterpret_cast<uintptr_t>(addr);
						if (dist < min_dist)
						{
							next_alloc = &alloc;
							min_dist = dist;
						}
					}
				}
				return next_alloc;
			}

			[[nodiscard]] void* allocate(size_t size) noexcept
			{
				auto* slot = getFreeAllocationSlot();
				if (slot != nullptr)
				{
					auto i = reinterpret_cast<uint8_t*>(this) + sizeof(BlockInfo);
					auto end = reinterpret_cast<uint8_t*>(this) + BlockSize;
					while (i < end)
					{
						auto* alloc = getAllocationAt(i);
						if (alloc != nullptr)
						{
							i += alloc->size;
						}
						else
						{
							auto* next_alloc = getNextAllocation(i);
							void* space_end = next_alloc == nullptr ? end : next_alloc->addr;
							auto space_size = reinterpret_cast<uintptr_t>(space_end) - reinterpret_cast<uintptr_t>(i);
							if (space_size >= size)
							{
								slot->addr = i;
								slot->size = size;
								return i;
							}
							i = reinterpret_cast<uint8_t*>(space_end);
						}
					}
				}
				return nullptr;
			}
		};

		inline static BlockInfo** blocks = nullptr;

		static void* operator new(size_t size)
		{
			if (blocks == nullptr)
			{
				blocks = (BlockInfo**)calloc(MaxBlocks, sizeof(BlockInfo*));
			}
			for (auto i = blocks; i != &blocks[MaxBlocks]; ++i)
			{
				if (*i != nullptr)
				{
					void* addr = (*i)->allocate(size);
					if (addr != nullptr)
					{
						return addr;
					}
				}
			}
			auto* new_block = (BlockInfo*)::malloc(BlockSize);
			void* addr = reinterpret_cast<uint8_t*>(new_block) + sizeof(BlockInfo);
			new_block->allocs[0].addr = addr;
			new_block->allocs[0].size = size;
			ZeroMemory(&new_block->allocs[1].addr, sizeof(BlockInfo) - offsetof(BlockInfo, allocs[1].addr));
			for (auto i = blocks; i != &blocks[MaxBlocks]; ++i)
			{
				if (*i == nullptr)
				{
					*i = new_block;
					break;
				}
			}
			return addr;
		}

		static void operator delete(void* addr)
		{
			bool had_prev = false;
			for (auto i = blocks; i != &blocks[MaxBlocks]; ++i)
			{
				if (*i != nullptr)
				{
					auto* alloc = (*i)->getAllocationAt(addr);
					if (alloc == nullptr)
					{
						had_prev = true;
					}
					else
					{
						alloc->addr = nullptr;
						if ((*i)->isEmpty())
						{
							::free(*i);
							*i = nullptr;
							if (!had_prev)
							{
								while (++i != &blocks[MaxBlocks])
								{
									if (*i != nullptr)
									{
										break;
									}
								}
								::free(blocks);
								blocks = nullptr;
							}
						}
						break;
					}
				}
			}
		}
	};
}
