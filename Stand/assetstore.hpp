#pragma once

#include "fwPool.hpp"

namespace rage
{
	template <typename T>
	struct fwGenericAssetDef
	{
		T* m_pObject;
		int m_refCount;
		hash_t m_name;
	};

	template <typename T>
	struct fwAssetNameDef : public fwGenericAssetDef<T/*, atFinalHashString*/>
	{
	};

	template <typename T, typename S /*= fwAssetDef<T>*/>
	struct fwAssetStore
	{
		PAD(0, 0x38) fwPool<S> m_pool;

		[[nodiscard]] S* GetSlot(/* strLocalIndex */ int index) const
		{
			return m_pool.GetSlot(index);
		}
	};
}