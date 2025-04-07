#pragma once

namespace Stand
{
	enum JoinMethod : long long
	{
		JM_NUTS, // Has no join confirmation.
		JM_NINJA, // Has join confirmation. Breaks when joining a job: Our screen keeps flashing, host keeps showing we're "joining" even tho on our end it shows "joined."
		JM_MATCHMAKING, // Has no join confirmation.
		JM_FRIEND, // Has join confirmation. Lets the target know we're joining via CGameInviteReply.

		JM_NUTS_REJOIN,

		JM_DEFAULT = JM_NUTS
	};
}
