#include "netConnectionManager.hpp"

#include "Exceptional.hpp"
#include "netConnection.hpp"
#include "netEndpoint.hpp"
#include "pointers.hpp"

#include "conf.hpp"
#include "main.hpp"

namespace rage
{
	using namespace Stand;

	netEndpoint* netConnectionManager::getEndpointById(EndpointId endpoint_id) noexcept
	{
		netEndpoint* res = nullptr;
		critical_section.Enter();
		__try
		{
			res = m_EndpointsByEndpointId.find(endpoint_id);
		}
		__EXCEPTIONAL()
		{
		}
		critical_section.Leave();
		return res;
	}

	netEndpoint* netConnectionManager::getEndpointByStartupId(netPeerId startup_id) noexcept
	{
		netEndpoint* res = nullptr;
		critical_section.Enter();
		__try
		{
			res = m_EndpointsByPeerId.find(startup_id);
		}
		__EXCEPTIONAL()
		{
		}
		critical_section.Leave();
		return res;
	}

	Cxn* netConnectionManager::GetCxnById(int id) noexcept
	{
		Cxn* res = nullptr;
		critical_section.Enter();
		__try
		{
			const int realId = (id & 0xFFFF);
			res = ((id >= 0 && unsigned(realId) < cxns.size) ? &cxns[realId] : 0);
			if (res != nullptr
				&& res->id != id
				)
			{
				res = nullptr;
			}
		}
		__EXCEPTIONAL()
		{
		}
		critical_section.Leave();
		return res;
	}

	bool netConnectionManager::Send(int32_t cxnId, const void* bytes, uint32_t numBytes, uint32_t sendFlags, netSequence* frameSeq)
	{
#if true
		return pointers::rage_netConnectionManager_Send(this, cxnId, bytes, numBytes, sendFlags, frameSeq);
#else
		bool ret = false;
		critical_section.Enter();
		__try
		{
			if (auto cxn = GetCxnById(cxnId))
			{
				ret = cxn->QueueFrame(bytes, numBytes, sendFlags, frameSeq);
			}
		}
		__EXCEPTIONAL()
		{
		}
		critical_section.Leave();
		return ret;
#endif
	}

	bool netConnectionManager::SendOutOfBand(const netAddress& addr, uint32_t channelId, const void* bytes, uint32_t numBytes, uint32_t sendFlags)
	{
		bool ret = false;
		critical_section.Enter();
		__try
		{
			ret = pointers::rage_netConnectionManager_SendConnectionless(this, addr, channelId, bytes, numBytes, sendFlags);
		}
		__EXCEPTIONAL()
		{
		}
		critical_section.Leave();
		return ret;
	}
}
