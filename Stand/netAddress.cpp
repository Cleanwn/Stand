#include "netAddress.hpp"

namespace rage
{
	bool netAddress::operator<(const netAddress& rhs) const
	{
		if (this->m_Type < rhs.m_Type)
		{
			return 1;
		}
		if (this->m_Type == rhs.m_Type)
		{
			switch (this->m_Type)
			{
			case TYPE_DIRECT:
				if (this->sock_addr.ip.value < rhs.sock_addr.ip.value)
					return 1;
				if (this->sock_addr.ip.value != rhs.sock_addr.ip.value)
					return 0;
				return this->sock_addr.port < rhs.sock_addr.port;

			case TYPE_RELAY_SERVER:
				if (this->proxy_sock_addr.ip.value >= rhs.proxy_sock_addr.ip.value
					&& (this->proxy_sock_addr.ip.value != rhs.proxy_sock_addr.ip.value || this->proxy_sock_addr.port >= rhs.proxy_sock_addr.port))
				{
					bool v6 = 0;
					if (rhs.proxy_sock_addr.ip.value < this->proxy_sock_addr.ip.value
						|| rhs.proxy_sock_addr.ip.value == this->proxy_sock_addr.ip.value && rhs.proxy_sock_addr.port < this->proxy_sock_addr.port)
					{
						return v6;
					}
					if (memcmp(&this->m_RelayToken, &rhs.m_RelayToken, 6ui64) >= 0)
						return v6;
				}
				return 1;

			case TYPE_PEER_RELAY:
				if (this->proxy_sock_addr.ip.value < rhs.proxy_sock_addr.ip.value
					|| this->proxy_sock_addr.ip.value == rhs.proxy_sock_addr.ip.value && this->proxy_sock_addr.port < rhs.proxy_sock_addr.port)
				{
					return 1;
				}
				if (rhs.proxy_sock_addr.ip.value < this->proxy_sock_addr.ip.value
					|| rhs.proxy_sock_addr.ip.value == this->proxy_sock_addr.ip.value && rhs.proxy_sock_addr.port < this->proxy_sock_addr.port)
				{
					return 0;
				}
				return this->peerId < rhs.peerId;
			}
		}
		return 0;
	}
}
