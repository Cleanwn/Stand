#include "netPeerAddress.hpp"

#include <soup/base64.hpp>

#include "datBitBuffer.hpp"
#include "netConnectionManager.hpp"
#include "netEndpoint.hpp"
#include "pointers.hpp"

namespace rage
{
	using namespace Stand;

	void netPeerAddress::censorOnlineIp() noexcept
	{
		local_only_online.ip.value = -1;
		local_only_online.port = 0;
	}

	uint32_t netPeerAddress::getMacId() const
	{
		return startup_id >> 32;
	}

	netSocketAddress netPeerAddress::getAddress() const
	{
		auto connect_addr = getEndpointAddress();
		/*if (CidrSupernets::_private.contains(connect_addr.ip.value))
		{
			return online;
		}*/
		return connect_addr;
	}

	netEndpoint* netPeerAddress::getEndpoint() const
	{
		return (*pointers::connection_mgr)->getEndpointByStartupId(startup_id);
	}

	netSocketAddress netPeerAddress::getEndpointAddress() const
	{
		if (auto endpoint = getEndpoint())
		{
			return endpoint->getRemoteAddress();
		}
		return {};
	}

	netSocketAddress netPeerAddress::getPublicAddress() const
	{
		if (auto endpoint = getEndpoint())
		{
			return endpoint->getPublicAddress();
		}
		return {};
	}

	void netPeerAddress::reset()
	{
		ZeroMemory(this, sizeof(*this));
	}

	void netPeerAddress::fromBase64(const std::string& str)
	{
		fromBinary(soup::base64::decode(str));
	}

	void netPeerAddress::fromBinary(const std::string& str)
	{
		datImportBuffer bb;
		bb.SetReadOnlyBytes(str.data(), (uint32_t)str.size());
		//ser(bb);
		pointers::rage_datImport_netPeerAddress(bb, *this);
	}
}
