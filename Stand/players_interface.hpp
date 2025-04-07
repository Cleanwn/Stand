#pragma once

#include "rgsc_common.hpp"

namespace rgsc
{
	struct IPlayerManagerV1 : public IRgscUnknown
	{
		virtual RGSC_HRESULT RGSC_CALL CreatePlayerEnumerator(/*RGSC_REFIID riid,
														 const RockstarId rockstarId,
														 const PlayerListType playerListType,
														 const u32 firstPlayerIndex,
														 const u32 maxPlayersToRead,
														 u32* numBytesRequired,
														 void** handle*/) = 0;

		virtual RGSC_HRESULT RGSC_CALL EnumeratePlayers(/*void* handle,
			void* buffer,
			const u32 bufferSize,
			IPlayerList** players,
			IAsyncStatus* status*/) = 0;

		virtual bool RGSC_CALL IsFriend(const RockstarId rockstarId) = 0;
		virtual RGSC_HRESULT RGSC_CALL ShowFriendRequestUi(const RockstarId rockstarId) = 0;
		virtual RGSC_HRESULT RGSC_CALL ShowPlayerProfileUi(const RockstarId rockstarId) = 0;
		virtual RGSC_HRESULT RGSC_CALL SendInvite(/*const RockstarId rockstarId, IAsyncStatus* status*/) = 0;
		virtual bool RGSC_CALL IsBlocked(const RockstarId rockstarId) = 0;
	};
}
