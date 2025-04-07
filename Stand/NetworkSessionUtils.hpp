#pragma once

enum HostFlags
{
	HF_IsPrivate = 1 << 0,
	HF_NoMatchmaking = 1 << 1,
	HF_SingleplayerOnly = 1 << 2,
	HF_Closed = 1 << 3,
	HF_ClosedFriends = 1 << 4,
	HF_ClosedCrew = 1 << 5,
	HF_AllowPrivate = 1 << 6,
	HF_ViaMatchmaking = 1 << 7,
	HF_ViaScript = 1 << 8,
	HF_ViaJoinFailure = 1 << 9,
	HF_ViaQuickmatchNoQueries = 1 << 10,
	HF_ViaRetry = 1 << 11,
	HF_ViaScriptToGame = 1 << 12,
	HF_JobToJob = 1 << 13, // to game only
	HF_Asynchronous = 1 << 14,
	HF_Premium = 1 << 15,
	HF_ViaTransitionLobby = 1 << 16, // to game only
	HF_JoinViaPresenceDisabled = 1 << 17,
	HF_JoinInProgressDisabled = 1 << 18,
};
