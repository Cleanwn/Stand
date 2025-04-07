#include "TcpConnection.hpp"

#include <fmt/format.h>

#include <soup/Socket.hpp>

namespace Stand
{
	soup::Socket& TcpConnection::getSocket() const noexcept
	{
		return *reinterpret_cast<soup::Socket*>(workers.at(0).get());
	}

	void TcpConnection::close()
	{
		if (!workers.empty())
		{
			getSocket().close();
		}
	}

	int TcpConnection::getLastError() noexcept
	{
		return WSAGetLastError();
	}

	std::string TcpConnection::errorCodeToString(int error_code) noexcept
	{
		/*switch (error_code)
		{
		case 10060:
			return "Connection timed out";
		}*/
		return fmt::to_string(error_code);
	}

	void TcpConnection::onSendError(int code)
	{
		//util::toast(std::string("send failed with error ").append(fmt::to_string(code)).append(" :|"), TOAST_ALL, false);
		close();
	}

	void TcpConnection::sendRaw(const std::string& str)
	{
		if (!workers.empty())
		{
			getSocket().send(str);
		}
	}
}
