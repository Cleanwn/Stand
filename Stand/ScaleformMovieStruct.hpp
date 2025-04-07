#pragma once

#define RAGE_MAX_PATH 256

enum eState
{
	SF_MOVIE_STATE_INACTIVE = 0,		// completely off/unused/deleted etc
	SF_MOVIE_STATE_FLAGGED_FOR_USE,		// not set up yet, but flagged to be used
	SF_MOVIE_STATE_STREAMING_MOVIE,		// loading and waiting on movie itself to stream
	SF_MOVIE_STATE_ACTIVE,				// active & considered in use
	SF_MOVIE_STATE_RESTARTING,
	SF_MOVIE_STATE_SET_TO_DELETE_PENDING_FINAL_UPDATE,		// movie is going to get set to be deleted via the RT but can still update and invoke/callback
	SF_MOVIE_STATE_SET_TO_REMOVE_STAGE_1,					// movie is going to get set to be deleted via the RT and can no longer update/callback
	SF_MOVIE_STATE_SET_TO_REMOVE_STAGE_2,					// movie is going to get set to be deleted without any further checks
	SF_MOVIE_STATE_SET_TO_REMOVE_NO_REF_CHECK  // goes into this state after ref check so we can retry on the assert and get others
};

struct ScaleformMovieStruct
{
	PAD(0, 0xB0) int iMovieId;
	/* 0xB4 */ int iParentMovie;
	/* 0xB8 */ char cFilename[RAGE_MAX_PATH];
	/* 0x1B8 */ eState iState;
	PAD(0x1B8 + 4, 0x1E0);
};
static_assert(sizeof(ScaleformMovieStruct) == 0x1E0);
// CScaleformMgr::FindMovieByFilename - 8B 8C 30 B8 01 00 00 FF C9
