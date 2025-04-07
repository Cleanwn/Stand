#pragma once

#include "typedecl.hpp"

// Can not be used with saved state, hotkeys, & saved commands because it is temporary.
#define CMDFLAG_TEMPORARY					(commandflags_t)0b1

// Command supports getState, setState & applyDefaultState
#define CMDFLAG_SUPPORTS_STATE_OPERATIONS	(commandflags_t)0b10

// Modifier for CMDFLAG_SUPPORTS_STATE_OPERATIONS: Command has saved state, but it should appear to the user as if it does not. Currently only used for CommandListWarning.
#define CMDFLAG_NO_USER_STATE_OPERATIONS	(commandflags_t)0b100

// Tells saved state & root check routines to finish iterating the list this command is a part of.
#define CMDFLAG_STATE_AND_CHECK_FINISHLIST	(commandflags_t)0b1000

// Tells search routines to finish iterating the list this command is a part of.
#define CMDFLAG_SEARCH_FINISHLIST			(commandflags_t)0b10000

// Tells search to ignore this command but still iterate its children.
#define CMDFLAG_SEARCH_CHILDRENONLY			(commandflags_t)0b100000

#define CMDFLAG_FEATURELIST_MASK			(commandflags_t)0b111000000

// Tells feature list routines to finish iterating the list this command is a part of.
#define CMDFLAG_FEATURELIST_FINISHLIST		(commandflags_t)0b001000000

// Tells feature list routines to skip this command.
#define CMDFLAG_FEATURELIST_SKIP			(commandflags_t)0b010000000

// Tells feature list routines to indicate this command as exclusive to Basic Edition and above.
#define CMDFLAG_FEATURELIST_BASIC			(commandflags_t)0b100000000

// Tells feature list routines to indicate this command as exclusive to Regular Edition and above.
#define CMDFLAG_FEATURELIST_REGULAR			(commandflags_t)0b101000000

// Tells feature list routines to indicate this command as exclusive to Ultimate Edition.
#define CMDFLAG_FEATURELIST_ULTIMATE		(commandflags_t)0b110000000

// Internal use only.
#define CMDFLAG_CONCEALED					(commandflags_t)0b1000000000

// If getStateCommand were to return this command, return nullptr instead.
#define CMDFLAG_NOT_STATE_FOR_CHILDREN		(commandflags_t)0b10000000000

// If set, onTickInViewport will not be dispatched for children.
#define CMDFLAG_CUSTOM_ON_TICK_IN_VIEWPORT	(commandflags_t)0b100000000000

// Tells hotkey routines to finish iterating the list this command is a part of.
#define CMDFLAG_HOTKEYS_FINISHLIST			(commandflags_t)0b1000000000000

// This command supports hotkeys and correlations and is therefore an excellent point to introduce the user to the context menu.
#define CMDFLAG_TUTORIAL_CTX_POINT			(commandflags_t)0b10000000000000

// This command does not want to be affected by bulk state operations.
// Additionally, if it has CMDFLAG_SUPPORTS_STATE_OPERATIONS, this will cause the context menu to only show "Apply Default State".
#define CMDFLAG_NO_SAVED_STATE				(commandflags_t)0b100000000000000

// Tells "saved commands" routines to finish iterating the list this command is a part of.
#define CMDFLAG_STARRED_FINISHLIST			(commandflags_t)0b1000000000000000

#define CMDFLAGS_ACTION						0
#define CMDFLAGS_ACTION_NAME				CMDFLAGS_ACTION
#define CMDFLAGS_ACTION_SCRIPT				CMDFLAGS_ACTION
#define CMDFLAGS_ACTION_SLIDER				0
#define CMDFLAGS_ACTION_VALUE				CMDFLAG_SUPPORTS_STATE_OPERATIONS
#define CMDFLAGS_COLOUR_CUSTOM				CMDFLAG_SUPPORTS_STATE_OPERATIONS
#define CMDFLAGS_COLOUR						CMDFLAGS_COLOUR_CUSTOM
#define CMDFLAGS_FLOW_EVENT					CMDFLAGS_LIST
#define CMDFLAGS_LIST						0
#define CMDFLAGS_LIST_ACTION				0
#define CMDFLAGS_LIST_CONCEALER				CMDFLAGS_LIST
#define CMDFLAGS_LIST_READONLY				CMDFLAGS_LIST
#define CMDFLAGS_LIST_SELECT				CMDFLAG_SUPPORTS_STATE_OPERATIONS
#define CMDFLAGS_LIST_WARNING				CMDFLAG_SUPPORTS_STATE_OPERATIONS | CMDFLAG_NO_USER_STATE_OPERATIONS
#define CMDFLAGS_READONLY_VALUE				0
#define CMDFLAGS_READONLY_VALUE_COPY		CMDFLAGS_READONLY_VALUE
#define CMDFLAGS_SEARCH						CMDFLAGS_LIST
#define CMDFLAGS_SLIDER						CMDFLAG_SUPPORTS_STATE_OPERATIONS
#define CMDFLAGS_SLIDER_TEXT				CMDFLAG_SUPPORTS_STATE_OPERATIONS
#define CMDFLAGS_TOGGLE_NO_CORRELATION		CMDFLAG_SUPPORTS_STATE_OPERATIONS
#define CMDFLAGS_TOGGLE						CMDFLAGS_TOGGLE_NO_CORRELATION
#define CMDFLAGS_TOGGLE_CUSTOM				0
