#pragma once

#include "rtti_shim.hpp"

namespace rage
{
	class sysMemAllocator
	{
	public:
		RAGE_RTTI_SHIM;
		virtual ~sysMemAllocator() = 0;
		virtual void SetQuitOnFail(bool) = 0;
		virtual void* Allocate(size_t size, size_t align, int subAllocator) = 0;
		virtual void* TryAllocate(size_t size, size_t align, int subAllocator) = 0;
		virtual void Free(void* pointer) = 0;
		virtual void DeferredFree(void* pointer) = 0;
		virtual void Resize(void* pointer, size_t size) = 0;
		virtual sysMemAllocator* GetAllocator(int allocator) const = 0;
		virtual sysMemAllocator* GetAllocator(int allocator) = 0;
		virtual sysMemAllocator* GetPointerOwner(void* pointer) = 0;
		virtual size_t GetSize(void* pointer) const = 0;
		virtual size_t GetMemoryUsed(int memoryBucket) = 0;
		virtual size_t GetMemoryAvailable() = 0;
		virtual void GetLargestAvailableBlock() = 0;
		virtual void GetLowWaterMark() = 0;
		virtual void GetHighWaterMark() = 0;
		virtual void UpdateMemorySnapshot() = 0;
		virtual void GetMemorySnapshot() = 0;
		virtual void IsTallied() = 0;
		virtual void BeginLayer() = 0;
		virtual void EndLayer() = 0;
		virtual void BeginMemoryLog() = 0;
		virtual void EndMemoryLog() = 0;
		virtual void IsBuildingResource() = 0;
		virtual void HasMemoryBuckets() = 0;
		virtual void SanityCheck() = 0;
		virtual bool IsValidPointer(void* pointer) = 0;
		virtual void SupportsAllocateMap() = 0;
		virtual void AllocateMap() = 0;
		virtual void FreeMap() = 0;
		virtual void GetSizeWithOverhead() = 0;
		virtual void GetHeapSize() = 0;
		virtual void GetHeapBase() = 0;
		virtual void SetHeapBase() = 0;
		virtual void IsRootResourceAllocation() = 0;
		virtual void GetCanonicalBlockPtr() = 0;
		virtual void TryLockBlock() = 0;
		virtual void UnlockBlock() = 0;
		virtual void GetUserData() = 0;
		virtual void SetUserData() = 0;
		virtual void GetMemoryDistribution() = 0;
		virtual void Defragment() = 0;
		virtual void GetFragmentation() = 0;
	};
	static_assert(sizeof(sysMemAllocator) == 8);

	class sysMemSimpleAllocator : public sysMemAllocator
	{
	};
}
