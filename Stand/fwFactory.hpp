#pragma once

namespace rage
{
	class fwFactoryBase
	{
	public:
		virtual ~fwFactoryBase() = default;
		virtual void* CreateBaseItem(int32_t storageBlockId) = 0;
		virtual uint32_t GetCount(int32_t storageBlockId) = 0;
		virtual uint32_t GetCapacity(int32_t storageBlockId) = 0;
		virtual uint32_t AddStorageBlock(int32_t storageBlockId, uint32_t size) = 0;
	};
}
