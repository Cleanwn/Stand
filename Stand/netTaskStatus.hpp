#pragma once

namespace rage
{
	enum netTaskStatus : int
	{
		NET_TASKSTATUS_NONE,
		NET_TASKSTATUS_PENDING,
		NET_TASKSTATUS_FAILED,
		NET_TASKSTATUS_SUCCEEDED
	};
}
