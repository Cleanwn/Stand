#pragma once

#include <cstdint>

enum eJoinResponseCode : int32_t
{
	RESPONSE_ACCEPT = 0,						// Join successful
	RESPONSE_DENY_UNKNOWN = 1,					// Unknown - Indicates bug - Use Generic Error
	RESPONSE_DENY_WRONG_SESSION = 2,			// Different session - Indicates bug - Use Generic Error 
	RESPONSE_DENY_NOT_HOSTING = 3,				// User not hosting - (might have left and come back) - Use Generic Error
	RESPONSE_DENY_NOT_READY = 4,				// Session still starting - would occur if someone hosted and immediately sent an invite.
	//      We start straight away so this should be very difficult in practice. Use Generic Error
	RESPONSE_DENY_BLACKLISTED = 5,				// Gamer is blacklisted from session
	RESPONSE_DENY_INVALID_REQUEST_DATA = 6,		// Indicates bug - Use Generic Error 
	RESPONSE_DENY_INCOMPATIBLE_ASSETS = 7,		// Different assets 
	RESPONSE_DENY_SESSION_FULL = 8,				// Session is full
	RESPONSE_DENY_GROUP_FULL = 9,				// Selected group is full
	RESPONSE_DENY_WRONG_VERSION = 10,			// Different session - Dev only - Use Generic Error
	RESPONSE_DENY_NOT_VISIBLE = 11,				// Session not visible for Matchmaking - Won't happen for Invites (would be a bug)
	//		Use Generic Error
	RESPONSE_DENY_BLOCKING = 12,					// Session has been blocked for invites by host (this would be set by script and, 
	//		for instance, might be to block players joining during a race)
	RESPONSE_DENY_AIM_PREFERENCE = 13,			// Different aim settings
	RESPONSE_DENY_CHEATER = 14,					// Cheater setting different (either local or session is flagged as cheater)
	RESPONSE_DENY_TIMEOUT = 15,					// Different time out - Dev only - Use Generic Error
	RESPONSE_DENY_DATA_HASH = 16,				// Different data hash - Indicates cloud assets different
	RESPONSE_DENY_CREW_LIMIT = 17,				// Too many crews
	RESPONSE_DENY_POOL_NORMAL = 18,				// Pool we tried to join is NORMAL. Player must be bad sport or cheater
	RESPONSE_DENY_POOL_BAD_SPORT = 19,			// Pool we tried to join is BAD SPORT. Player must be normal or cheater
	RESPONSE_DENY_POOL_CHEATER = 20,				// Pool we tried to join is CHEATER. Player must be normal or bad sport
	RESPONSE_DENY_NOT_JOINABLE = 21,				// Session no longer exists
	RESPONSE_DENY_PRIVATE_ONLY = 22,				// Session has only private slots. Player was not invited. 
	___UNK_ADDED_IN_3095,
	RESPONSE_DENY_DIFFERENT_BUILD,			// Different build type - Dev Only
	RESPONSE_DENY_DIFFERENT_PORT,			// Different socket port - Dev Only
	RESPONSE_DENY_DIFFERENT_CONTENT_SETTING,// Different content setting (online vs. offline)
	RESPONSE_DENY_NOT_FRIEND,               // A player who is not a friend of the host is trying to join a friends-only session
	RESPONSE_DENY_REPUTATION,				// Reputation of players is different (One is bad, other is good). Xbox One only.
	RESPONSE_DENY_FAILED_TO_ESTABLISH,		// Couldn't contact the session. It may no longer exist
	RESPONSE_DENY_PREMIUM,					// Premium session, no access
	RESPONSE_SOLO,							// Solo session
	RESPONSE_DENY_ADMIN_BLOCKED,			// Admin blocked due to no notification
	RESPONSE_DENY_TOO_MANY_BOSSES			// Too many bosses in the session
};
