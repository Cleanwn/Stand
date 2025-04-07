#pragma once

#include "assetstore.hpp"
#include "pgBase.hpp"

namespace rage
{
	struct sfScaleformMovie;

	struct sfScaleformMovieView
	{
		PAD(0, 0x10) GFxMovieView* m_MovieView; // GPtr<GFxMovieView>
	};
}

struct CScaleformMovieObject : public rage::pgBase
{
	/* 0x10 */ rage::sfScaleformMovie* m_Movie;
	/* 0x18 */ rage::sfScaleformMovieView* m_MovieView;
	/* 0x20 */ int m_MemoryArena;

	[[nodiscard]] GFxMovieView* GetMovieView()
	{
		if (m_MovieView)
		{
			return m_MovieView->m_MovieView;
		}
		return nullptr;
	}
};

struct CScaleformDef : public rage::fwAssetNameDef<CScaleformMovieObject>
{
	INIT_PAD(rage::fwAssetNameDef<CScaleformMovieObject>, 0xD8);
};
static_assert(sizeof(CScaleformDef) == 0xD8); // m_nStorageSize

struct CScaleformStore : public rage::fwAssetStore<CScaleformMovieObject, CScaleformDef>
{
	[[nodiscard]] GFxMovieView* GetMovieView(/* strLocalIndex */ int iIndex)
	{
		CScaleformDef* pDef = GetSlot(iIndex);
		if (pDef && pDef->m_pObject)
		{
			return pDef->m_pObject->GetMovieView();
		}
		return nullptr;
	}
};
