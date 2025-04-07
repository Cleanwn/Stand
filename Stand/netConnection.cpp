#include "netConnection.hpp"
#if I_CAN_SPY

#include "pointers.hpp"

namespace rage
{
	using namespace Stand;

	bool netConnection::QueueFrame(const void* bytes, uint32_t numBytes, uint32_t sendFlags, netSequence* frameSeq)
	{
		return pointers::rage_netConnection_QueueFrame(this, bytes, numBytes, sendFlags, frameSeq);
	}
}
#endif
